#include <nltools/messaging/Messaging.h>
#include <nltools/messaging/Message.h>
#include <nltools/logging/Log.h>
#include "TestDriver.h"
#include <glib.h>
#include <nltools/threading/Expiration.h>

struct MessagingTests
{
  static void doMainLoop(std::chrono::milliseconds minTime, std::chrono::milliseconds timeout,
                         std::function<bool()> test)
  {
    Expiration exp;
    exp.refresh(timeout);

    Expiration min;

    if(minTime != std::chrono::milliseconds::zero())
      min.refresh(minTime);

    while((exp.isPending() && !test()) || min.isPending())
      g_main_context_iteration(nullptr, TRUE);

    g_assert(test());
  }

  static void registerTests()
  {
    using namespace std::chrono_literals;

    g_test_add_func("/Messaging/init-deinit", [] {
      using namespace nltools::msg;

      nltools::Log::setLevel(nltools::Log::Debug);

      for(int i = 0; i < 100; i++)
      {
        Configuration conf{ { EndPoint::TestEndPoint }, { EndPoint::TestEndPoint } };
        nltools::msg::init(conf);
      }
    });

    g_test_add_func("/Messaging/send-receive", [] {
      using namespace nltools::msg;

      Configuration conf{ { EndPoint::TestEndPoint }, { EndPoint::TestEndPoint } };
      nltools::msg::init(conf);

      int numMessages = 0;
      ParameterChangedMessage msgToSend(12, 0.3);
      assert(waitForConnection(EndPoint::TestEndPoint));
      auto c = receive<ParameterChangedMessage>(EndPoint::TestEndPoint, [&](const auto &msg) { numMessages++; });
      send(EndPoint::TestEndPoint, msgToSend);
      doMainLoop(0s, 2s, [&] { return numMessages == 0; });
      c.disconnect();
    });

    g_test_add_func("/Messaging/no-packets-lost-if-bombed", [] {
      using namespace nltools::msg;

      nltools::Log::setLevel(nltools::Log::Debug);

      Configuration conf{ { EndPoint::TestEndPoint }, { EndPoint::TestEndPoint } };
      nltools::msg::init(conf);

      int numRecMessages = 0;
      int numSendMessages = 100;

      ParameterChangedMessage msgToSend(12, 0.3);
      assert(waitForConnection(EndPoint::TestEndPoint));
      auto c = receive<ParameterChangedMessage>(EndPoint::TestEndPoint, [&](const auto &msg) { numRecMessages++; });

      for(int i = 0; i < numSendMessages; i++)
        send(EndPoint::TestEndPoint, msgToSend);

      doMainLoop(0s, 5s, [&] { return numRecMessages == numSendMessages; });
      c.disconnect();
    });

    g_test_add_func("/Messaging/notify-on-discovery", [] {
      using namespace nltools::msg;

      bool received = false;
      Configuration conf{ { EndPoint::TestEndPoint }, { EndPoint::TestEndPoint } };
      nltools::msg::init(conf);
      auto c = onConnectionEstablished(EndPoint::TestEndPoint, [&] { received = true; });
      doMainLoop(0s, 1s, [&] { return received; });
      c.disconnect();
    });
  }
};

static TestDriver<MessagingTests> driver;
