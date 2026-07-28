#include "WifiManager.h"
#include "config.h"

WifiManager::WifiManager(const char* ssid, const char* password)
    : ssid_(ssid), password_(password) {}

void WifiManager::beginConnect() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true, true);
  delay(100);
  WiFi.begin(ssid_, password_);
  connecting_ = true;
  lastReconnectAttemptMs_ = millis();
}

bool WifiManager::connect(unsigned long timeoutMs) {
  beginConnect();

  const unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - start) < timeoutMs) {
    delay(250);
  }

  connecting_ = (WiFi.status() != WL_CONNECTED);
  return isConnected();
}

void WifiManager::update() {
  if (isConnected()) {
    connecting_ = false;
    return;
  }

  const unsigned long now = millis();
  if (!connecting_ || (now - lastReconnectAttemptMs_) >= Config::kReconnectIntervalMs) {
    beginConnect();
  }
}

bool WifiManager::isConnected() const {
  return WiFi.status() == WL_CONNECTED;
}

WifiStatusSnapshot WifiManager::snapshot() const {
  WifiStatusSnapshot snap{};
  snap.status = WiFi.status();
  snap.statusName = statusToString(snap.status);
  snap.ssid = ssid_;
  snap.connected = (snap.status == WL_CONNECTED);

  if (snap.connected) {
    snap.ip = WiFi.localIP().toString();
    snap.gateway = WiFi.gatewayIP().toString();
    snap.rssi = WiFi.RSSI();
  } else {
    snap.ip = "-";
    snap.gateway = "-";
    snap.rssi = 0;
  }

  return snap;
}

const char* WifiManager::statusToString(wl_status_t status) {
  switch (status) {
    case WL_IDLE_STATUS:     return "IDLE";
    case WL_NO_SSID_AVAIL:   return "NO_SSID";
    case WL_SCAN_COMPLETED:  return "SCAN_DONE";
    case WL_CONNECTED:       return "CONNECTED";
    case WL_CONNECT_FAILED:  return "CONNECT_FAILED";
    case WL_CONNECTION_LOST: return "CONNECTION_LOST";
    case WL_DISCONNECTED:    return "DISCONNECTED";
    default:                 return "UNKNOWN";
  }
}
