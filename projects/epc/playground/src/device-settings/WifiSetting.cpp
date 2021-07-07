#include "WifiSetting.h"
#include <nltools/system/SpawnCommandLine.h>
#include <nltools/Types.h>
#include <nltools/logging/Log.h>
#include <glibmm.h>

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
             "accesspoint;"
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
             "accesspoint;"
             "systemctl",
             "mask",
             "accesspoint;" };
};

WifiSetting::WifiSetting(UpdateDocumentContributor& settings, const std::shared_ptr<EpcWifi>& localWifi)
    : super(settings, WifiSettings::Querying)
    , m_localWifi(localWifi)
{
  static auto isEpc2 = !strcmp(TARGET_PLATFORM, "epc2");
  if(isEpc2)
    m_pollImpl = std::make_unique<EPC2WiFiPollImpl>(this);
  else
    m_pollImpl = std::make_unique<BBBWiFiPollImpl>(this);

  pollAccessPointRunning();
}

bool WifiSetting::set(tEnum m)
{
  auto ret = super::set(m);

  if(m == WifiSettings::Enabled)
    m_localWifi->setNewWifiState(true);
  else if(m == WifiSettings::Disabled)
    m_localWifi->setNewWifiState(false);

  enableDisableBBBWifi(m);
  pollAccessPointRunning();

  return ret;
}

bool WifiSetting::persistent() const
{
  return false;
}

bool WifiSetting::pollAccessPointRunning()
{
  if constexpr(!isDevelopmentPC)
  {
    if(m_pollImpl)
    {
      return m_pollImpl->poll();
    }
  }

  return false;
}

void WifiSetting::enableDisableBBBWifi(WifiSettings m)
{
  auto bbbWifiEnableDisableArgs = getArgs(m);
  SpawnAsyncCommandLine::spawn(
      bbbWifiEnableDisableArgs, [](auto str) { nltools::Log::error(__LINE__, "succ:", str); },
      [](auto err) { nltools::Log::error(__LINE__, "err:", err); });
}

WiFiPollImpl::WiFiPollImpl(WifiSetting* setting)
    : m_setting { setting }
{
}

bool EPC2WiFiPollImpl::poll()
{
  SpawnAsyncCommandLine::spawn(
      { "nmcli", "-g", "GENERAL.STATE", "con", "show", "C15" },
      [this](const std::string& ret)
      {
        nltools::Log::error("nmcli says:", ret);
        m_setting->set(ret == "activated" ? WifiSettings::Enabled : WifiSettings::Disabled);
      },
      [](const std::string& ret) { nltools::Log::error(ret); });
  return false;
}

bool BBBWiFiPollImpl::poll()
{
  m_pollConnection.disconnect();

  GPid pid;
  std::vector<std::string> argsBBB { "/usr/bin/ssh", "-o",        "StrictHostKeyChecking=no", "root@192.168.10.11",
                                     "systemctl",    "is-active", "accesspoint.service" };
  Glib::spawn_async("", argsBBB, Glib::SPAWN_DO_NOT_REAP_CHILD, Glib::SlotSpawnChildSetup(), &pid);
  m_pollConnection = Glib::MainContext::get_default()->signal_child_watch().connect(
      sigc::mem_fun(this, &BBBWiFiPollImpl::onPollReturned), pid);
  return false;
}

void BBBWiFiPollImpl::onPollReturned(GPid pid, int exitStatus)
{
  if(exitStatus == 0)
  {
    m_setting->set(WifiSettings::Enabled);
  }
  else if(exitStatus == (3 << 8))
  {
    m_setting->set(WifiSettings::Disabled);
  }
  else
  {
    nltools::Log::warning("Polling wifi status failed, poll again in 2 secs.");
    Glib::MainContext::get_default()->signal_timeout().connect_seconds(sigc::mem_fun(this, &BBBWiFiPollImpl::poll), 2);
  }
}
