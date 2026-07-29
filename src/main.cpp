#include <Arduino.h>

#include "RfidRc522.h"
#include "WifiManager.h"
#include "config.h"
#include "secrets.h"

namespace {
WifiManager wifi(Secrets::kWifiSsid, Secrets::kWifiPassword);

RfidRc522 rfid({
    Config::kRfidSsPin,
    Config::kRfidRstPin,
    Config::kRfidSckPin,
    Config::kRfidMisoPin,
    Config::kRfidMosiPin,
});

unsigned long lastStatusPrintMs = 0;

const byte kAllowedUids[][4] = {
    {0x69, 0xAF, 0xFD, 0xD5},
};
constexpr size_t kAllowedUidCount =
    sizeof(kAllowedUids) / sizeof(kAllowedUids[0]);

void setLedConnected(bool connected) {
  // На многих ESP32-C3 LED активен низким уровнем
  digitalWrite(Config::kLedPin, connected ? LOW : HIGH);
}

void printWifiStatus(const WifiStatusSnapshot& snap) {
  Serial.println("---------- WiFi status ----------");
  Serial.printf("SSID:     %s\n", snap.ssid.c_str());
  Serial.printf("Status:   %s (%d)\n", snap.statusName, snap.status);

  if (snap.connected) {
    Serial.printf("IP:       %s\n", snap.ip.c_str());
    Serial.printf("RSSI:     %d dBm\n", snap.rssi);
    Serial.printf("Gateway:  %s\n", snap.gateway.c_str());
  } else {
    Serial.println("IP:       -");
    Serial.println("RSSI:     -");
  }

  Serial.println("---------------------------------");
  Serial.flush();
}

void printRfidStatus() {
  const byte ver = rfid.firmwareVersion();
  Serial.println("---------- RFID status ----------");
  Serial.printf("Online:   %s\n", rfid.isOnline() ? "YES" : "NO");
  Serial.printf("Version:  0x%02X\n", ver);
  Serial.printf("Pins:     SS=%u RST=%u SCK=%u MISO=%u MOSI=%u\n",
                rfid.pins().ss, rfid.pins().rst, rfid.pins().sck,
                rfid.pins().miso, rfid.pins().mosi);
  Serial.println("Поднеси метку — появится RFID UID: ...");
  Serial.println("---------------------------------");
  Serial.flush();
}


constexpr unsigned long kRelayHoldTimeoutMs = 2500;

bool relayOn = false;
unsigned long lastAllowedSeenMs = 0;

void setRelay(bool on) {
  if (relayOn == on) {
    return;
  }
  relayOn = on;
  digitalWrite(Config::Rele_Pin, on ? HIGH : LOW);
  Serial.println(on ? "RFID: реле ON" : "RFID: реле OFF");
  Serial.flush();
}

void handleRfid() {
  RfidUid uid;
  if (rfid.tryRead(uid)) {
    Serial.printf("RFID UID: %s\n", uid.toHex().c_str());

    if (rfid.isAllowed(uid, kAllowedUids, kAllowedUidCount)) {
      lastAllowedSeenMs = millis();
      setRelay(true);
    } else {
      Serial.println("RFID DENY");
      setRelay(false);
    }
    Serial.flush();
  }

  // Метка пропала из зоны — выключаем реле
  if (relayOn && (millis() - lastAllowedSeenMs) >= kRelayHoldTimeoutMs) {
    setRelay(false);
  }
}
}  // namespace

void setup() {
  pinMode(Config::kLedPin, OUTPUT);
  pinMode(Config::Rele_Pin, OUTPUT);
  digitalWrite(Config::Rele_Pin, LOW);
  setLedConnected(false);

  Serial.begin(Config::kSerialBaud);
  delay(Config::kUsbCdcSettleMs);

  Serial.println();
  Serial.println("=================================");
  Serial.println("ESP32-C3 WiFi + RFID-RC522");
  Serial.println("SSID/password: include/secrets.h");
  Serial.println("=================================");
  Serial.flush();

  if (rfid.begin()) {
    Serial.printf("RFID: RC522 OK (version 0x%02X)\n", rfid.firmwareVersion());
  } else {
    Serial.printf("RFID: НЕТ СВЯЗИ (version 0x%02X) — проверь 3.3V и SPI-провода\n",
      rfid.firmwareVersion());
  }
  printRfidStatus();

  const bool ok = wifi.connect(Config::kConnectTimeoutMs);
  Serial.println(ok ? "WiFi connected" : "WiFi timeout, retrying in background");
  printWifiStatus(wifi.snapshot());
  setLedConnected(ok);

  lastStatusPrintMs = millis();
}

void loop() {
  //wifi.update();
  handleRfid();

  const unsigned long now = millis();
  if (now - lastStatusPrintMs >= Config::kStatusIntervalMs) {
    lastStatusPrintMs = now;

 //   const WifiStatusSnapshot snap = wifi.snapshot();
 //   setLedConnected(snap.connected);
 //  printWifiStatus(snap);
    printRfidStatus();
  }
}
