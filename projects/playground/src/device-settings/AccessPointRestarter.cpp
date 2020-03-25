#include "AccessPointRestarter.h"
#include "DebugLevel.h"
#include <nltools/system/SpawnCommandLine.h>

AccessPointRestarter &AccessPointRestarter::get()
{
  static AccessPointRestarter restarter;
  return restarter;
}

AccessPointRestarter::AccessPointRestarter()
    : m_throttler(std::chrono::seconds(1))
    , m_done { false }
{
}

AccessPointRestarter::~AccessPointRestarter()
{
  join();
}

void AccessPointRestarter::join()
{
  if(m_thread && m_thread->joinable())
  {
    DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__);
    m_thread->join();
    m_thread.reset();
    DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__);
  }
}

void AccessPointRestarter::scheduleRestart()
{
  DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__);

  if(m_done)
    join();

  m_throttler.doTask([=] {
    if(m_thread)
    {
      scheduleRestart();
      DebugLevel::warning(__PRETTY_FUNCTION__, "old request still processing, re-scheduled");
    }
    else
    {
      DebugLevel::warning(__PRETTY_FUNCTION__, "start background process to restart access point");
      m_done = false;
      m_thread = std::make_unique<std::thread>([=] {
        this->restartAP();
        m_done = true;
      });
    }
  });
}

void AccessPointRestarter::restartAP()
{
  DebugLevel::warning(__PRETTY_FUNCTION__);
#if _DEVELOPMENT_PC
  std::this_thread::sleep_for(std::chrono::seconds(3));
#else
  SpawnCommandLine cmd(R"(ssh -o "StrictHostKeyChecking=no" root@192.168.10.11 'systemctl restart accesspoint')");
#endif
}
