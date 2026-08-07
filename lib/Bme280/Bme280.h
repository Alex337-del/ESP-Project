#pragma once

#include <Arduino.h>
#include <Adafruit_BME280.h>

struct Bme280Config {
  uint8_t sda;
  uint8_t scl;
  uint8_t i2cAddr;  // обычно 0x76, запасной 0x77
};

struct Bme280Reading {
  float temperatureC = NAN;
  float humidityPercent = NAN;
  float pressureHpa = NAN;
  bool valid = false;
};

class Bme280 {
 public:
  explicit Bme280(const Bme280Config& config);

  // I2C + поиск адреса + init. true — датчик отвечает.
  bool begin();

  uint8_t address() const { return address_; }
  bool isOnline() const { return online_; }

  // Считать температуру, влажность и давление.
  bool read(Bme280Reading& out);

  float temperatureC();
  float humidityPercent();
  float pressureHpa();

  Adafruit_BME280& driver() { return bme_; }

 private:
  uint8_t probeAddress() const;

  Bme280Config config_;
  Adafruit_BME280 bme_;
  uint8_t address_ = 0;
  bool online_ = false;
};
