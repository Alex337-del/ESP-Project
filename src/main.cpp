#include <Arduino.h>

#include "WifiManager.h"
#include "config.h"
#include "secrets.h"

namespace {
WifiManager wifi(Secrets::kWifiSsid, Secrets::kWifiPassword);
unsigned long lastStatusPrintMs = 0;

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
}  // namespace

void setup() {
  pinMode(Config::kLedPin, OUTPUT);
  setLedConnected(false);

  Serial.begin(Config::kSerialBaud);
  delay(Config::kUsbCdcSettleMs);

  Serial.println();
  Serial.println("=================================");
  Serial.println("ESP32-C3 WiFi monitor started");
  Serial.println("SSID/password: include/secrets.h");
  Serial.println("=================================");
  Serial.flush();

  const bool ok = wifi.connect(Config::kConnectTimeoutMs);
  Serial.println(ok ? "WiFi connected" : "WiFi timeout, retrying in background");
  printWifiStatus(wifi.snapshot());
  setLedConnected(ok);

  lastStatusPrintMs = millis();
}

void loop() {
  wifi.update();

  const unsigned long now = millis();
  if (now - lastStatusPrintMs >= Config::kStatusIntervalMs) {
    lastStatusPrintMs = now;

    const WifiStatusSnapshot snap = wifi.snapshot();
    setLedConnected(snap.connected);
    printWifiStatus(snap);
  }
}
