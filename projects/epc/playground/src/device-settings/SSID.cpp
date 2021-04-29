#include <device-settings/DeviceName.h>
#include <device-settings/Settings.h>
#include <device-settings/SSID.h>
//#include "SSID.h"

SSID::SSID(Settings &parent, const std::shared_ptr<Wifi>& shrd_ptr)
    : Setting(parent)
    , m_wifi(shrd_ptr)
{
  parent.getSetting<DeviceName>()->onChange([=](const Setting *s) {
    static const std::string dict = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-_";

    std::string ssid = "NL-C15-" + s->save();

    for(char &it : ssid)
      if(dict.find(it) == std::string::npos)
        it = '_';

//    setSSIDAndNotifyBBB(ssid);
    updateSSID(ssid);
  });
}

SSID::~SSID() = default;

void SSID::load(const Glib::ustring &ssid, Initiator initiator)
{
//  setSSIDAndNotifyBBB(ssid);
  updateSSID(ssid);
}

Glib::ustring SSID::save() const
{
  return m_ssid;
}

bool SSID::persistent() const
{
  return true;
}

Glib::ustring SSID::getDisplayString() const
{
  return m_ssid;
}

void SSID::updateSSID(const Glib::ustring &str)
{
  m_ssid = str;

  auto ssidMsg = nltools::msg::WiFi::SetWiFiSSIDMessage(m_ssid);
  nltools::msg::send(nltools::msg::EndPoint::BeagleBone, ssidMsg);

  m_wifi->setSSID(m_ssid);

  notify();
}
