#pragma once

#include <array>
#include <thread>
#include <atomic>

#ifndef __arm__
#include <emmintrin.h>
#endif

namespace nltools
{
  namespace threading
  {
    enum class Priority
    {
      Normal,
      AlmostRealtime,
      Realtime
    };

    void setThisThreadPrio(Priority p);

#ifndef __arm__
    // based on work of Timur Doumler
    // to be found here: https://timur.audio/using-locks-in-real-time-audio-processing-safely

    struct spinning_condition_variable
    {
      spinning_condition_variable()
      {
        flag.test_and_set(std::memory_order_acquire);
      }

      void wait() noexcept
      {
        while(true)
        {
          for(int i = 0; i < 1000; ++i)
          {
            if(test())
              return;

            _mm_pause();
            _mm_pause();
            _mm_pause();
            _mm_pause();
            _mm_pause();
            _mm_pause();
            _mm_pause();
            _mm_pause();
            _mm_pause();
            _mm_pause();
          }

          std::this_thread::yield();
        }
      }

      void notify() noexcept
      {
        flag.clear(std::memory_order_release);
      }

     private:
      bool test() noexcept
      {
        return !flag.test_and_set(std::memory_order_acquire);
      }

      std::atomic_flag flag = ATOMIC_FLAG_INIT;
    };
#endif
  }
}