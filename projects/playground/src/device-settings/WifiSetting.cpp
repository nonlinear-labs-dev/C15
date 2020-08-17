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

WifiSetting::WifiSetting(UpdateDocumentContributor& settings)
    : super(settings, WifiSettings::Querying)
{
  pollAccessPointRunning();
}

bool WifiSetting::set(tEnum m)
{
  auto ret = super::set(m);

  if(!isDevelopmentPC)
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
  if(!isDevelopmentPC)
  {
    std::vector<std::string> args { "/usr/bin/ssh", "-o",        "StrictHostKeyChecking=no", "root@192.168.10.11",
                                    "systemctl",    "is-active", "accesspoint.service" };

    GPid pid;
    try
    {
      Glib::spawn_async("", args, Glib::SPAWN_DO_NOT_REAP_CHILD, Glib::SlotSpawnChildSetup(), &pid);
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
