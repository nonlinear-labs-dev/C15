#include <fstream>
#include <giomm.h>
#include <thread>
#include "WiFiManager.h"

WiFiManager::WiFiManager()
{
  nltools::msg::receive<nltools::msg::WiFi::SetWiFiSSIDMessage>(nltools::msg::EndPoint::WiFiManager,
                                                                [this](const auto& msg) {
                                                                  m_lastSeenSSID = msg.m_ssid.get();
                                                                  saveConfig();
                                                                });

  nltools::msg::receive<nltools::msg::WiFi::SetWiFiPasswordMessage>(nltools::msg::EndPoint::WiFiManager,
                                                                    [this](const auto& msg) {
                                                                      m_lastSeenPassword = msg.m_password.get();
                                                                      saveConfig();
                                                                    });

  nltools::msg::onConnectionEstablished(nltools::msg::EndPoint::Playground, [this] { sendMessages(); });
  readConfig();
}

void WiFiManager::readConfig()
{
  auto file = "/etc/hostapd.conf";
  if(auto stream = std::ifstream(file))
  {
    auto line = std::string {};
    bool changed = false;

    while(std::getline(stream, line))
    {
      if(line.find(c_ssidPattern) == 0)
      {
        auto ssidOnDisk = line.substr(line.find_first_of('=') + 1);
        changed |= std::exchange(m_lastSeenSSID, ssidOnDisk) != ssidOnDisk;
      }
      else if(line.find(c_pwPattern) == 0)
      {
        auto pwOnDisk = line.substr(line.find_first_of('=') + 1);
        changed |= std::exchange(m_lastSeenPassword, pwOnDisk) != pwOnDisk;
      }
    }

    if(changed)
      sendMessages();
  }
}

void WiFiManager::sendMessages() const
{
  nltools::Log::info("Sending Password and SSID messages! SSID:", m_lastSeenSSID, "Password:", m_lastSeenPassword);

  auto pwMessage = nltools::msg::WiFi::WiFiPasswordChangedMessage(m_lastSeenPassword);
  auto ssidMessage = nltools::msg::WiFi::WiFiSSIDChangedMessage(m_lastSeenSSID);

  nltools::msg::send(nltools::msg::EndPoint::Playground, pwMessage);
  nltools::msg::send(nltools::msg::EndPoint::Playground, ssidMessage);
}

void WiFiManager::saveConfig()
{

  try
  {
    std::string line;
    std::stringstream in(Glib::file_get_contents(c_fileName));
    std::stringstream out;

    const auto oldContent = in.str();

    while(std::getline(in, line))
    {
      if(line.find(c_pwPattern) == 0)
        out << c_pwPattern << "=" << m_lastSeenPassword << std::endl;
      else if(line.find(c_ssidPattern) == 0)
        out << c_ssidPattern << "=" << m_lastSeenSSID << std::endl;
      else
        out << line << std::endl;
    }

    auto newContent = out.str();

    if(oldContent != newContent)
    {
      if(auto configFile = std::ofstream(c_fileName, std::ios_base::trunc))
      {
        configFile << newContent;
        scheduleRestart();
      }
    }
  }
  catch(...)
  {
  }
}

void WiFiManager::scheduleRestart()
{
#ifndef _DEVELOPMENT_PC
  nltools::Log::info(__FILE__, __FUNCTION__, "schedule Restart!");
  auto thread = std::thread([]() {
    system("systemctl restart accesspoint");
    nltools::Log::info(__FILE__, __FUNCTION__, "WiFi Restarted!");
  });
  thread.detach();
#endif
}
