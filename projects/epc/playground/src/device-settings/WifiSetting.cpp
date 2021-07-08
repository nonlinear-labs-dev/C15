#include "WifiSetting.h"
#include <nltools/system/SpawnCommandLine.h>
#include <nltools/Types.h>
#include <nltools/logging/Log.h>
#include <playground.h>
#include <utility>

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
}

bool WifiSetting::set(tEnum m)
{
  Environment::getStackTrace(std::to_string(__LINE__) + " " + toString(m));
  auto ret = super::set(m);

  if(m == WifiSettings::Enabled)
    m_localWifi->setNewWifiState(true);
  else if(m == WifiSettings::Disabled)
    m_localWifi->setNewWifiState(false);

  enableDisableBBBWifi(m);

  return ret;
}

bool WifiSetting::persistent() const
{
  return true;
}

void WifiSetting::enableDisableBBBWifi(WifiSettings m)
{
  if constexpr(!isDevelopmentPC) {
    auto bbbWifiEnableDisableArgs = getArgs(m);
    SpawnAsyncCommandLine::spawn(bbbWifiEnableDisableArgs,
      [](auto str) { nltools::Log::error(__LINE__, "succ:", str); },
      [](auto err) { nltools::Log::error(__LINE__, "err:", err); });
  }
}