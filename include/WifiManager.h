#pragma once

#include <Arduino.h>
#include <WiFi.h>

struct WifiStatusSnapshot {
  wl_status_t status;
  const char* statusName;
  String ssid;
  String ip;
  String gateway;
  int32_t rssi;
  bool connected;
};

class WifiManager {
 public:
  WifiManager(const char* ssid, const char* password);

  // Блокирующее подключение (для setup)
  bool connect(unsigned long timeoutMs);

  // Неблокирующий опрос: держит соединение, переподключается при обрыве
  void update();

  bool isConnected() const;
  WifiStatusSnapshot snapshot() const;

  static const char* statusToString(wl_status_t status);

 private:
  void beginConnect();

  const char* ssid_;
  const char* password_;
  unsigned long lastReconnectAttemptMs_ = 0;
  bool connecting_ = false;
};
