#pragma once

#include <nltools/threading/Throttler.h>
#include <thread>
#include <atomic>

class AccessPointRestarter
{
 public:
  static AccessPointRestarter &get();
  ~AccessPointRestarter();

  void scheduleRestart();

 private:
  AccessPointRestarter();
  void restartAP();
  void join();

  Throttler m_throttler;
  std::unique_ptr<std::thread> m_thread;
  std::atomic<bool> m_done;
};
