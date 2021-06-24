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
  pollAccessPointRunning();
}

bool WifiSetting::set(tEnum m)
{
  auto ret = super::set(m);

  if(m == WifiSettings::Enabled)
    m_localWifi->setNewWifiState(true);
  else if(m == WifiSettings::Disabled)
    m_localWifi->setNewWifiState(false);

  if constexpr(!isDevelopmentPC)
  {
    m_pollConnection.disconnect();

    GPid commandPid {};
    Glib::spawn_async("", getArgs(m), Glib::SPAWN_DO_NOT_REAP_CHILD, Glib::SlotSpawnChildSetup(), &commandPid);
    Glib::MainContext::get_default()->signal_child_watch().connect(sigc::mem_fun(this, &WifiSetting::onCommandReturned),
                                                                   commandPid);
  }

  return ret;
}

void WifiSetting::onCommandReturned(GPid, int exitStatus)
{
  if(exitStatus != 0)
    pollAccessPointRunning();
}

bool WifiSetting::persistent() const
{
  return false;
}

bool WifiSetting::pollAccessPointRunning()
{
  if constexpr(!isDevelopmentPC)
  {
    std::vector<std::string> argsBBB { "/usr/bin/ssh", "-o",        "StrictHostKeyChecking=no", "root@192.168.10.11",
                                       "systemctl",    "is-active", "accesspoint.service" };
    std::vector<std::string> argsEPC2 { "systemctl", "is-active", "accesspoint.service" };
    static auto isEpc2 = !strcmp(TARGET_PLATFORM, "epc2");
    GPid pid;
    try
    {
      if(isEpc2)
      {
        Glib::spawn_async("", argsEPC2, Glib::SPAWN_DO_NOT_REAP_CHILD, Glib::SlotSpawnChildSetup(), &pid);
      }
      else
      {
        Glib::spawn_async("", argsBBB, Glib::SPAWN_DO_NOT_REAP_CHILD, Glib::SlotSpawnChildSetup(), &pid);
      }
      m_pollConnection = Glib::MainContext::get_default()->signal_child_watch().connect(
          sigc::mem_fun(this, &WifiSetting::onPollReturned), pid);
    }
    catch(const Glib::SpawnError& e)
    {
      nltools::Log::error(e.what());
    }
  }

  return false;
}

void WifiSetting::onPollReturned(GPid pid, int exitStatus)
{
  if(exitStatus == 0)
  {
    super::set(WifiSettings::Enabled);
  }
  else if(exitStatus == (3 << 8))
  {
    super::set(WifiSettings::Disabled);
  }
  else
  {
    nltools::Log::warning("Polling wifi status failed, poll again in 2 secs.");
    Glib::MainContext::get_default()->signal_timeout().connect_seconds(
        sigc::mem_fun(this, &WifiSetting::pollAccessPointRunning), 2);
  }
}
