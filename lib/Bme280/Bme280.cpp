#include "Bme280.h"

#include <Wire.h>

Bme280::Bme280(const Bme280Config& config) : config_(config) {}

uint8_t Bme280::probeAddress() const {
  const uint8_t candidates[] = {config_.i2cAddr, 0x76, 0x77};
  for (uint8_t addr : candidates) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      return addr;
    }
  }
  return 0;
}

bool Bme280::begin() {
  Wire.begin(config_.sda, config_.scl);
  Wire.setClock(100000);
  Wire.setTimeOut(50);

  address_ = probeAddress();
  if (address_ == 0) {
    address_ = config_.i2cAddr;
  }

  online_ = bme_.begin(address_, &Wire);
  if (!online_ && address_ != 0x77) {
    address_ = 0x77;
    online_ = bme_.begin(address_, &Wire);
  }
  if (!online_ && address_ != 0x76) {
    address_ = 0x76;
    online_ = bme_.begin(address_, &Wire);
  }

  return online_;
}

bool Bme280::read(Bme280Reading& out) {
  out = {};
  if (!online_) {
    return false;
  }

  out.temperatureC = bme_.readTemperature();
  out.humidityPercent = bme_.readHumidity();
  out.pressureHpa = bme_.readPressure() / 100.0f;
  out.valid = !isnan(out.temperatureC) && !isnan(out.humidityPercent);
  return out.valid;
}

float Bme280::temperatureC() {
  return online_ ? bme_.readTemperature() : NAN;
}

float Bme280::humidityPercent() {
  return online_ ? bme_.readHumidity() : NAN;
}

float Bme280::pressureHpa() {
  return online_ ? (bme_.readPressure() / 100.0f) : NAN;
}
