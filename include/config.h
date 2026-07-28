#pragma once

// Аппаратные настройки платы
namespace Config {
  constexpr int kLedPin = 8;
  constexpr unsigned long kSerialBaud = 115200;
  constexpr unsigned long kUsbCdcSettleMs = 2000;

  // Период вывода статуса в Serial
  constexpr unsigned long kStatusIntervalMs = 5000;

  // Таймаут первой попытки подключения в setup()
  constexpr unsigned long kConnectTimeoutMs = 20000;

  // Интервал между попытками переподключения в loop()
  constexpr unsigned long kReconnectIntervalMs = 10000;
}
