#pragma once
#include <thread>
#include <condition_variable>
#include <atomic>

class USBManager
{
 public:
  USBManager();
  ~USBManager();

 private:
  std::mutex cv_m;
  std::atomic_bool m_running { true };
  std::thread m_backgroundThread;
  std::condition_variable m_condition;

  static bool usbAvailable();
  static bool updateAvailable();
};
