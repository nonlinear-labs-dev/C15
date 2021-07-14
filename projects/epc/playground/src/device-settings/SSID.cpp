#include <device-settings/DeviceName.h>
#include <device-settings/Settings.h>
#include <device-settings/SSID.h>

SSID::SSID(Settings &parent, const std::shared_ptr<EpcWifi> &localWifi)
    : Setting(parent)
    , m_localWifi(localWifi)
{
  parent.getSetting<DeviceName>()->onChange([=](const Setting *s) {
    static const std::string dict = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-_";

    std::string ssid = "NL-C15-" + s->save();

    for(char &it : ssid)
      if(dict.find(it) == std::string::npos)
        it = '_';

    updateSSID(ssid);
  });
}

SSID::~SSID() = default;

void SSID::load(const Glib::ustring &ssid, Initiator initiator)
{
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

  static auto isEpc2 = !strcmp(TARGET_PLATFORM, "epc2");
  static auto suffix = (isEpc2 ? "_BBB" : "");

  auto ssidMsg = nltools::msg::WiFi::SetWiFiSSIDMessage(m_ssid + suffix);
  nltools::msg::send(nltools::msg::EndPoint::BeagleBone, ssidMsg);

  m_localWifi->setNewSSID(m_ssid);

  notify();
}
