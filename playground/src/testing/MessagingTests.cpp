#include <nltools/messaging/Messaging.h>
#include <nltools/messaging/Message.h>
#include <nltools/logging/Log.h>
#include "TestDriver.h"
#include <glib.h>
#include <nltools/threading/Expiration.h>

struct MessagingTests
{
  static void doMainLoop(std::chrono::milliseconds timeout, std::function<bool()> test)
  {
    Expiration exp;
    exp.refresh(timeout);

    while(exp.isPending() && !test())
      g_main_context_iteration(nullptr, TRUE);

    g_assert(test());
  }

  static void registerTests()
  {
    using namespace std::chrono_literals;

    g_test_add_func("/Messaging/send-receive", [] {
      using namespace nltools::msg;

      Configuration conf{ { EndPoint::TestEndPoint }, { EndPoint::TestEndPoint } };
      nltools::msg::init(conf);

      int numMessages = 0;
      ParameterChangedMessage msgToSend(12, 0.3);
      assert(waitForConnection(EndPoint::TestEndPoint));
      auto c = receive<ParameterChangedMessage>(EndPoint::TestEndPoint, [&](const auto &msg) { numMessages++; });
      send(EndPoint::TestEndPoint, msgToSend);
      doMainLoop(2s, [&] { return numMessages == 0; });
    });

    g_test_add_func("/Messaging/no-packets-lost-if-bombed", [] {
      using namespace nltools::msg;

      Configuration conf{ { EndPoint::TestEndPoint }, { EndPoint::TestEndPoint } };
      nltools::msg::init(conf);

      int numRecMessages = 0;
      int numSendMessages = 1000;

      ParameterChangedMessage msgToSend(12, 0.3);
      assert(waitForConnection(EndPoint::TestEndPoint));
      auto c = receive<ParameterChangedMessage>(EndPoint::TestEndPoint, [&](const auto &msg) { numRecMessages++; });

      for(int i = 0; i < numSendMessages; i++)
        send(EndPoint::TestEndPoint, msgToSend);

      doMainLoop(5s, [&] { return numRecMessages == numSendMessages; });
    });

    g_test_add_func("/Messaging/notify-on-discovery", [] {
      using namespace nltools::msg;

      bool received = false;
      Configuration conf{ { EndPoint::TestEndPoint }, { EndPoint::TestEndPoint } };
      nltools::msg::init(conf);
      auto c = onConnectionEstablished(EndPoint::TestEndPoint, [&] { received = true; });
      doMainLoop(1s, [&] { return received; });
    });
  }
};

static TestDriver<MessagingTests> driver;
