#include <WiFi.h>

#define WIFI_LOG_PREFIX "WiFi"

const unsigned long wiFiRetryInterval = 20000;
unsigned long wiFiLastAttemptTime = 0;
bool wiFiConnectedMessageShowed = false;

void connectToWiFi() {
  logInfo(WIFI_LOG_PREFIX, "Trying to connect to WiFi.");
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.begin(ESP32_WIFI_SSID, ESP32_WIFI_PASSWORD);
  setLcdMessage("Trying to connect to WiFi...", 0, wiFiRetryInterval);
}

bool isWiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

void loopWiFi() {
  wl_status_t status = WiFi.status();

  switch (status) {
    case WL_CONNECTED:
      if (!wiFiConnectedMessageShowed) {
        logInfo(WIFI_LOG_PREFIX, "Connected successfully to WiFi.");
        setLcdMessage("Connected successfully to WiFi.", 0, 6000);
        wiFiConnectedMessageShowed = true;
      }
      return;

    case WL_IDLE_STATUS:
      wiFiConnectedMessageShowed = false;
      return;

    case WL_DISCONNECTED:
    case WL_CONNECT_FAILED:
    case WL_CONNECTION_LOST:
    case WL_NO_SSID_AVAIL:
      wiFiConnectedMessageShowed = false;
      unsigned long attemptTime = millis();
      if (attemptTime - wiFiLastAttemptTime > wiFiRetryInterval) {
        wiFiLastAttemptTime = attemptTime;
        logInfo(WIFI_LOG_PREFIX, "WiFi disconnected, reconnecting...");
        connectToWiFi();
      }
      return;
  }
}