#pragma once

#include <mutex>
#include <condition_variable>
#include <atomic>

namespace nltools
{

  class BackgroundThreadWaiter
  {
   public:
    void wait()
    {
      std::unique_lock<std::mutex> l(m_mutex);

      while(!m_flag)
        m_condition.wait(l);
    }

    bool waitFor(std::chrono::milliseconds s)
    {
      std::unique_lock<std::mutex> l(m_mutex);

      if(!m_flag)
        m_condition.wait_for(l, s);

      return m_flag;
    }

    void notify()
    {
      std::unique_lock<std::mutex> l(m_mutex);
      m_flag = true;
      m_condition.notify_all();
    }

    bool isNotified() const
    {
      return m_flag;
    }

   private:
    std::condition_variable m_condition;
    std::mutex m_mutex;
    std::atomic<bool> m_flag = false;
  };
}
