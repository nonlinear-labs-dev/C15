#include "WifiSetting.h"
#include <nltools/system/SpawnCommandLine.h>
#include <nltools/Types.h>
#include <nltools/logging/Log.h>
#include <playground.h>
#include <utility>
#include <nltools/messaging/Messaging.h>

std::vector<std::string> getArgs(WifiSettings s)
{
  if(s == WifiSettings::Enabled)
    return { "/usr/bin/ssh",
             "-o",
             "StrictHostKeyChecking=no",
             "root@192.168.10.11",
             "systemctl",
             "unmask",
             "accesspoint;",
             "systemctl",
             "enable",
             "accesspoint;",
             "systemctl",
             "start",
             "accesspoint;" };
  else
    return { "/usr/bin/ssh",
             "-o",
             "StrictHostKeyChecking=no",
             "root@192.168.10.11",
             "systemctl",
             "stop",
             "accesspoint;",
             "systemctl",
             "disable",
             "accesspoint;",
             "systemctl",
             "mask",
             "accesspoint;" };
};

WifiSetting::WifiSetting(UpdateDocumentContributor& settings, std::shared_ptr<EpcWifi> localWifi)
    : super(settings, WifiSettings::Enabled)
    , m_localWifi(std::move(localWifi))
{
  nltools::msg::onConnectionEstablished(nltools::msg::EndPoint::BeagleBone, [this](){
     nltools::Log::error(__LINE__, "did Load?", m_didSettingLoad);
     m_connectionToBBBEstablished = true;
     setupBBBWifiIfBBBConnectedAndSettingLoaded();
  });
}

bool WifiSetting::set(tEnum m)
{
  Environment::getStackTrace(std::to_string(__LINE__));
  auto didChange = super::set(m);

  if(m == WifiSettings::Enabled)
    m_localWifi->setNewWifiState(true);
  else if(m == WifiSettings::Disabled)
    m_localWifi->setNewWifiState(false);

  if(!isLoading()) {
    setupBBBWifiIfBBBConnectedAndSettingLoaded();
  }

  return didChange;
}

bool WifiSetting::persistent() const
{
  return true;
}

bool WifiSetting::isLoading() const
{
  return m_isLoading;
}

void WifiSetting::load(const Glib::ustring& text, Initiator initiator)
{
  m_isLoading = true;
  NLEnumSetting::load(text, initiator);

  if(initiator == Initiator::EXPLICIT_LOAD) {
    m_didSettingLoad = true;
    setupBBBWifiIfBBBConnectedAndSettingLoaded();
  }

  m_isLoading = false;
}

void WifiSetting::enableDisableBBBWifi(WifiSettings m)
{
  if constexpr(!isDevelopmentPC) {

    auto bbbWifiEnableDisableArgs = getArgs(m);
    SpawnAsyncCommandLine::spawn(bbbWifiEnableDisableArgs,
      [](auto str) { nltools::Log::error(__LINE__, "success:", str); },
      [](auto err) { nltools::Log::error(__LINE__, "error:", err); });
  }
}

void WifiSetting::setupBBBWifiIfBBBConnectedAndSettingLoaded()
{
  Environment::getStackTrace(std::to_string(__LINE__));
  nltools::Log::error(__LINE__, "bbbb there:", m_connectionToBBBEstablished, "loaded", m_didSettingLoad);

  if(m_connectionToBBBEstablished && m_didSettingLoad)
    enableDisableBBBWifi(get());
}
