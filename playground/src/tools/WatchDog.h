#pragma once

#include "playground.h"
#include <chrono>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <nltools/threading/Expiration.h>

class WatchDog
{
 public:
  WatchDog();
  ~WatchDog();

  typedef std::function<void(int, int)> tCB;

  void run(std::chrono::steady_clock::duration timeout);
  void run(std::chrono::steady_clock::duration timeout, tCB cb);

  void stop();

 private:
  void onTimer();
  void doBackgroundCheck(tCB cb);
  void killProcess();
  bool isDebuggerPreset();

  std::chrono::steady_clock::duration m_timeout;
  Expiration m_regularTimer;
  std::atomic<int> m_counter = { 0 };
  std::atomic<bool> m_run = { 1 };

  std::thread m_bg;

  std::condition_variable m_condition;
  std::mutex m_mutex;
};
