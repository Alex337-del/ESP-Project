#pragma once

#include <Arduino.h>

// Аппаратные настройки платы
namespace Config {
  constexpr int kLedPin = 8;
  constexpr int Rele_Pin = 10;
  constexpr unsigned long kSerialBaud = 115200;
  constexpr unsigned long kUsbCdcSettleMs = 2000;

  // Период вывода статуса в Serial
  constexpr unsigned long kStatusIntervalMs = 5000;

  // Таймаут первой попытки подключения в setup()
  constexpr unsigned long kConnectTimeoutMs = 20000;

  // Интервал между попытками переподключения в loop()
  constexpr unsigned long kReconnectIntervalMs = 10000;

  // RFID-RC522 (SPI). Не пересекаются с LED(8) и реле(10).
  constexpr uint8_t kRfidSsPin = 7;
  constexpr uint8_t kRfidRstPin = 3;
  constexpr uint8_t kRfidSckPin = 4;
  constexpr uint8_t kRfidMisoPin = 5;
  constexpr uint8_t kRfidMosiPin = 6;


  
}
