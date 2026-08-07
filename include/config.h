#pragma once

#include <Arduino.h>

namespace Config {
  constexpr unsigned long kSerialBaud = 115200;
  constexpr unsigned long kUsbCdcSettleMs = 2000;

  // LCD1602 через модуль PCF8574 (I2C)
  // SDA -> GPIO2, SCL -> GPIO1
  constexpr uint8_t kLcdSdaPin = 2;
  constexpr uint8_t kLcdSclPin = 1;
  constexpr uint8_t kLcdI2cAddr = 0x27;
  constexpr uint8_t kLcdCols = 16;
  constexpr uint8_t kLcdRows = 2;

  // BME280 (I2C): SDA -> GPIO4, SCL -> GPIO3
  constexpr uint8_t kBmeSdaPin = 4;
  constexpr uint8_t kBmeSclPin = 3;
  constexpr uint8_t kBmeI2cAddr = 0x76;
  constexpr unsigned long kBmeReadIntervalMs = 2000;
}
