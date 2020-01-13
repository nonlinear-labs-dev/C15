#pragma once

#include <nltools/messaging/Messaging.h>
#include <nltools/threading/Expiration.h>
#include <proxies/audio-engine/AudioEngineProxy.h>

template <typename tMessageType> class PresetSenderAndReceiverWithTest
{
 public:
  PresetSenderAndReceiverWithTest()
  {
    using namespace nltools::msg;
    using namespace std::chrono_literals;

    Configuration conf{ { EndPoint::TestEndPoint }, { EndPoint::TestEndPoint } };
    m_oldConfig = nltools::msg::swapConfig(conf);

    nltools_assertInTest(waitForConnection(EndPoint::TestEndPoint));

    auto message = AudioEngineProxy::createMessage<tMessageType>();

    auto testPassed = false;
    auto c = nltools::msg::receive<tMessageType>(nltools::msg::EndPoint::TestEndPoint, [&](const tMessageType& msg) {
      auto ret = msg == message;
      REQUIRE(ret);
      testPassed = ret;
    });

    send(EndPoint::TestEndPoint, message);

    doMainLoop(0s, 2s, [&] { return testPassed; });
    c.disconnect();
  }

  ~PresetSenderAndReceiverWithTest()
  {
    nltools::msg::init(m_oldConfig);
  }

 private:
  void doMainLoop(std::chrono::milliseconds minTime, std::chrono::milliseconds timeout, std::function<bool()> test)
  {
    Expiration exp;
    exp.refresh(timeout);

    Expiration min;

    if(minTime != std::chrono::milliseconds::zero())
      min.refresh(minTime);

    while((exp.isPending() && !test()) || min.isPending())
      g_main_context_iteration(nullptr, TRUE);

    nltools::Log::notify(__LINE__, test());
  }

  nltools::msg::Configuration m_oldConfig;
};
