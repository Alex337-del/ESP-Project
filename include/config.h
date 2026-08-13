#pragma once

#include <Arduino.h>

namespace Config {
  constexpr unsigned long kSerialBaud = 115200;
  constexpr unsigned long kUsbCdcSettleMs = 2000;

  // Общая I2C-шина: SDA -> GPIO2, SCL -> GPIO1
  constexpr uint8_t kI2cSdaPin = 2;
  constexpr uint8_t kI2cSclPin = 1;

  // LCD1602 через PCF8574
  constexpr uint8_t kLcdI2cAddr = 0x27;
  constexpr uint8_t kLcdCols = 16;
  constexpr uint8_t kLcdRows = 2;

  // BME280
  constexpr uint8_t kBmeI2cAddr = 0x76;
  constexpr unsigned long kBmeReadIntervalMs = 2000;
}
