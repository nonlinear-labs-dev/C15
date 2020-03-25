#include <device-settings/DeviceName.h>
#include <device-settings/Settings.h>
#include <device-settings/SSID.h>

SSID::SSID(Settings &parent)
    : Setting(parent)
{
  nltools::msg::receive<nltools::msg::WiFi::WiFiSSIDChangedMessage>(nltools::msg::EndPoint::Playground,
                                                                    [this](const auto &msg) {
                                                                      m_ssid = msg.m_ssid.get();
                                                                      notify();
                                                                    });

  parent.getSetting<DeviceName>()->onChange([=](const Setting *s) {
    static const std::string dict = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-_";

    std::string ssid = "NL-C15-" + s->save();

    for(char &it : ssid)
      if(dict.find(it) == std::string::npos)
        it = '_';

    setSSIDAndNotifyBBB(ssid);
  });

  m_ssid = "Not Received Yet!";
}

SSID::~SSID() = default;

void SSID::load(const Glib::ustring &unused)
{
}

Glib::ustring SSID::save() const
{
  return m_ssid;
}

bool SSID::persistent() const
{
  return false;
}

Glib::ustring SSID::getDisplayString() const
{
  return m_ssid;
}

void SSID::setSSIDAndNotifyBBB(const Glib::ustring &str)
{
  m_ssid = str;

  auto ssidMsg = nltools::msg::WiFi::SetWiFiSSIDMessage(m_ssid);
  nltools::msg::send(nltools::msg::EndPoint::WiFiManager, ssidMsg);

  notify();
}
