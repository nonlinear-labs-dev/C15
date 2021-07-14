#include "USBManager.h"
#include <nltools/messaging/Message.h>
#include <nltools/system/SpawnCommandLine.h>

USBManager::USBManager()
    : m_backgroundThread { [this] {
      while(m_running)
      {
        std::unique_lock<std::mutex> lock(cv_m);

        nltools::msg::USB::USBStatusMessage msg {};
        msg.m_usbAvailable = usbAvailable();
        msg.m_updateAvailable = updateAvailable();
        nltools::msg::send(nltools::msg::EndPoint::Playground, msg);

        m_condition.wait_for(lock, std::chrono::seconds(5));
      }
    } }
{
}

USBManager::~USBManager()
{
  m_running = false;
  m_condition.notify_all();

  if(m_backgroundThread.joinable())
    m_backgroundThread.join();
}

bool USBManager::usbAvailable()
{
#ifndef _DEVELOPMENT_PC
  SpawnCommandLine cmd("lsblk");
  return cmd.getStdOutput().find("/mnt/usb-stick") != std::string::npos;
#else
  return true;
#endif
}

bool USBManager::updateAvailable()
{
  SpawnCommandLine cmd("ls /mnt/usb-stick/nonlinear-c15-update.tar");
  return cmd.getExitStatus() == 0;
}
