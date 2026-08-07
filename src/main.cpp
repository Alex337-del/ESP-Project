#include <Arduino.h>

#include "Bme280.h"
#include "config.h"

namespace {
Bme280 bme({
    Config::kBmeSdaPin,
    Config::kBmeSclPin,
    Config::kBmeI2cAddr,
});

unsigned long lastBmeReadMs = 0;
}  // namespace

void setup() {
  Serial.begin(Config::kSerialBaud);
  delay(Config::kUsbCdcSettleMs);

  Serial.println();
  Serial.println("=================================");
  Serial.println("ESP32-C3 + BME280");
  Serial.printf("I2C: SDA=GPIO%u SCL=GPIO%u\n",
                Config::kBmeSdaPin, Config::kBmeSclPin);
  Serial.flush();

  if (bme.begin()) {
    Serial.printf("BME280 OK, addr 0x%02X\n", bme.address());
  } else {
    Serial.printf("BME280 FAIL, tried 0x%02X — check 3.3V, GND, SDA=4, SCL=3\n",
                  bme.address());
  }
  Serial.flush();

  lastBmeReadMs = millis();
}

void loop() {
  const unsigned long now = millis();
  if (now - lastBmeReadMs < Config::kBmeReadIntervalMs) {
    return;
  }
  lastBmeReadMs = now;

  if (!bme.isOnline()) {
    Serial.println("BME280 offline, retry begin...");
    bme.begin();
    Serial.flush();
    return;
  }

  Bme280Reading reading;
  if (!bme.read(reading)) {
    Serial.println("BME280: read failed");
    Serial.flush();
    return;
  }

  Serial.println("---------- BME280 ----------");
  Serial.printf("Temp:     %.2f C\n", reading.temperatureC);
  Serial.printf("Humidity: %.2f %%\n", reading.humidityPercent);
  Serial.printf("Pressure: %.2f hPa\n", reading.pressureHpa);
  Serial.println("----------------------------");
  Serial.flush();
}
