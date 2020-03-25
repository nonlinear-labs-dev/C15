#include <device-settings/DebugLevel.h>
#include <fcntl.h>
#include <stdlib.h>
#include <nltools/system/SpawnCommandLine.h>
#include <tools/WatchDog.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <glibmm.h>
#include <profiling/Profiler.h>

WatchDog::WatchDog()
    : m_regularTimer(sigc::mem_fun(this, &WatchDog::onTimer))
{
}

WatchDog::~WatchDog()
{
  stop();
}

bool WatchDog::isDebuggerPreset()
{
  int fd = open("/proc/self/status", O_RDONLY);

  if(fd == -1)
    return false;

  char buffer[1024];
  ssize_t num_read = read(fd, buffer, sizeof(buffer));

  if(num_read > 0)
  {
    const char tracerPidPattern[] = "TracerPid:";
    auto sizeOfPattern = strlen(tracerPidPattern);

    buffer[num_read] = 0;
    char* tracerPidStr = strstr(buffer, tracerPidPattern);

    if(tracerPidStr && strlen(tracerPidStr) > sizeOfPattern)
    {
      auto tracersPid = atoi(tracerPidStr + sizeOfPattern);
      return tracersPid > 0;
    }
  }

  return false;
}

void WatchDog::run(std::chrono::steady_clock::duration timeout)
{
  run(timeout, [=](int numWarnings, int msInactive) {
    const int maxNumWarnings = 10;
    DebugLevel::error(G_STRLOC, " => MainLoop did not answer for more than", msInactive, "ms!");

#ifdef _PROFILING
    Profiler::get().printAllCallstacks();
#endif

    if(++numWarnings >= maxNumWarnings && !isDebuggerPreset())
    {
      killProcess();
    }
  });
}

void WatchDog::run(std::chrono::steady_clock::duration timeout, tCB cb)
{
  m_timeout = timeout;

  stop();

  m_run = true;

  m_bg = std::thread([=]() { doBackgroundCheck(cb); });

  m_regularTimer.refresh(m_timeout, Glib::PRIORITY_HIGH);
}

void WatchDog::doBackgroundCheck(tCB cb)
{
  auto timeOfLastChange = std::chrono::steady_clock::now();
  int numWarnings = 0;

  while(m_run)
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    auto oldVal = m_counter.load();

    m_condition.wait_for(lock, 2 * m_timeout);

    if(m_run)
    {
      if(oldVal == m_counter)
      {
        numWarnings++;
        auto now = std::chrono::steady_clock::now();
        auto timeSinceLastChange = now - timeOfLastChange;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(timeSinceLastChange).count();

        cb(numWarnings, ms);
      }
      else
      {
        numWarnings = 0;
        timeOfLastChange = std::chrono::steady_clock::now();
      }
    }
  }
}

void WatchDog::killProcess()
{
  DebugLevel::error(G_STRLOC, " => sending SIGSEGV to kill the process!");
  SpawnCommandLine cmdline("kill -11 " + to_string(getpid()));
}

void WatchDog::stop()
{
  m_run = false;

  if(m_bg.joinable())
  {
    m_condition.notify_all();
    m_bg.join();
  }
}

void WatchDog::onTimer()
{
  m_counter++;
  m_regularTimer.refresh(m_timeout, Glib::PRIORITY_HIGH);
}
