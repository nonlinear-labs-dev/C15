#include <catch.hpp>

#include <nltools/threading/Threading.h>
#include <nltools/logging/Log.h>
#include <future>

#ifndef __arm__

TEST_CASE("Spinning Condition Variable Ping Pong")
{
  nltools::threading::spinning_condition_variable s2r;
  nltools::threading::spinning_condition_variable r2s;

  auto numMessages = 1000;

  auto sender = std::async(std::launch::async, [&] {
    for(int i = 0; i < numMessages; i++)
    {
      s2r.notify();
      r2s.wait();
    }
  });

  auto receiver = std::async(std::launch::async, [&] {
    int r = 0;
    while(true)
    {
      s2r.wait();
      r2s.notify();

      if(++r == numMessages)
        break;
    }
  });

  receiver.wait();
  sender.wait();
}

#endif