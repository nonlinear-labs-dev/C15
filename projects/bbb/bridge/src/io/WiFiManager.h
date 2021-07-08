#pragma once
#include <nltools/messaging/Message.h>

class WiFiManager
{
 public:
  WiFiManager();

 private:
  void saveConfig();
  void scheduleRestart();

  std::string m_lastSeenSSID {};
  std::string m_lastSeenPassword {};

  constexpr static auto c_ssidPattern = "ssid";
  constexpr static auto c_pwPattern = "wpa_passphrase";
  constexpr static auto c_fileName = "/etc/hostapd.conf";
  void enableAndStartAP();
  void disableAndStopAP();
};
