#include <fstream>
#include <giomm.h>
#include <thread>
#include "WiFiManager.h"
#include <nltools/system/SpawnAsyncCommandLine.h>

WiFiManager::WiFiManager()
{
  nltools::msg::receive<nltools::msg::WiFi::SetWiFiSSIDMessage>(nltools::msg::EndPoint::BeagleBone,
                                                                [this](const auto& msg) {
                                                                  nltools::Log::notify("got ssid:", msg.m_ssid.get());
                                                                  m_lastSeenSSID = msg.m_ssid.get();
                                                                  saveConfig();
                                                                });

  nltools::msg::receive<nltools::msg::WiFi::SetWiFiPasswordMessage>(nltools::msg::EndPoint::BeagleBone,
                                                                    [this](const auto& msg) {
                                                                      nltools::Log::notify("got pw:", msg.m_password.get());
                                                                      m_lastSeenPassword = msg.m_password.get();
                                                                      saveConfig();
                                                                    });

  nltools::msg::receive<nltools::msg::WiFi::EnableWiFiMessage>(nltools::msg::EndPoint::BeagleBone, [this](const auto& msg) {
     if(msg.m_enable)
       enableAndStartAP();
     else
       disableAndStopAP();
  });
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
      if(line.find(c_pwPattern) == 0 && !m_lastSeenPassword.empty())
        out << c_pwPattern << "=" << m_lastSeenPassword << std::endl;
      else if(line.find(c_ssidPattern) == 0 && !m_lastSeenSSID.empty())
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
    nltools::Log::error("could not write host apd config!");
  }
}

void WiFiManager::scheduleRestart()
{
#ifndef _DEVELOPMENT_PC
  nltools::Log::info(__FILE__, __FUNCTION__, "schedule Restart!");
  auto thread = std::thread([]() {
    if(system("systemctl restart accesspoint"))
      nltools::Log::error("Could not restart WiFi!");
    else
      nltools::Log::notify(__FILE__, __FUNCTION__, "WiFi Restarted!");
  });
  thread.detach();
#endif
}

void WiFiManager::enableAndStartAP()
{
  std::vector<std::string> commands = {"/usr/C15/scripts/enableAndStartAP.sh"};

  SpawnAsyncCommandLine::spawn(commands, [](auto ret){
      nltools::Log::notify(__LINE__, ret);
  }, [](auto err) {
      nltools::Log::error(__LINE__, err);
  });
}

void WiFiManager::disableAndStopAP()
{
  std::vector<std::string> commands = {"/usr/C15/scripts/disableAndStopAP.sh"};

  SpawnAsyncCommandLine::spawn(commands, [](auto ret){
      nltools::Log::notify(__LINE__, ret);
    }, [](auto err) {
      nltools::Log::error(__LINE__, err);
    });
}
