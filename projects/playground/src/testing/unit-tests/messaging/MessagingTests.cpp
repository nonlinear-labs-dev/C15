#include <nltools/messaging/Messaging.h>
#include <nltools/messaging/Message.h>
#include <nltools/logging/Log.h>
#include <glib.h>
#include <nltools/threading/Expiration.h>
#include <testing/TestHelper.h>

namespace test_detail
{
  void doMainLoop(std::chrono::milliseconds minTime, std::chrono::milliseconds timeout,
                  const std::function<bool()> &test)
  {
    Expiration exp;
    exp.refresh(timeout);

    Expiration min;

    if(minTime != std::chrono::milliseconds::zero())
      min.refresh(minTime);

    while((exp.isPending() && !test()) || min.isPending())
      g_main_context_iteration(nullptr, TRUE);

    CHECK(test());
  }

}

using namespace nltools::msg;
using namespace std::chrono_literals;

TEST_CASE("Notify on discovery", "[Messaging][nltools]")
{
  bool received = false;
  auto c = onConnectionEstablished(EndPoint::TestEndPoint, [&] { received = true; });
  test_detail::doMainLoop(1s, 1s, [&] { return received; });
  c.disconnect();
}

TEST_CASE("Send Receive", "[Messaging][nltools]")
{
  int numMessages = 0;
  UnmodulateableParameterChangedMessage msgToSend { 12, 0.3, VoiceGroup::I };
  CHECK(waitForConnection(EndPoint::TestEndPoint));
  auto c
      = receive<UnmodulateableParameterChangedMessage>(EndPoint::TestEndPoint, [&](const auto &msg) { numMessages++; });
  send(EndPoint::TestEndPoint, msgToSend);
  test_detail::doMainLoop(1s, 1s, [&] { return numMessages == 1; });
  c.disconnect();
}

TEST_CASE("No packet lost if bombed", "[Messaging][nltools]")
{
  int numRecMessages = 0;
  int numSendMessages = 1000;

  UnmodulateableParameterChangedMessage msgToSend { 12, 0.3, VoiceGroup::I };
  CHECK(waitForConnection(EndPoint::TestEndPoint));
  auto c = receive<UnmodulateableParameterChangedMessage>(EndPoint::TestEndPoint,
                                                          [&](const auto &msg) { numRecMessages++; });

  for(int i = 0; i < numSendMessages; i++)
    send(EndPoint::TestEndPoint, msgToSend);

  test_detail::doMainLoop(1s, 1s, [&] { return numRecMessages == numSendMessages; });
  c.disconnect();
}

TEST_CASE("No packet doubles", "[Messaging][nltools]")
{
  int numRecMessages = 0;
  int numSendMessages = 100;

  UnmodulateableParameterChangedMessage msgToSend { 12, 0.3, VoiceGroup::I };
  CHECK(waitForConnection(EndPoint::TestEndPoint));
  auto c = receive<UnmodulateableParameterChangedMessage>(EndPoint::TestEndPoint,
                                                          [&](const auto &msg) { numRecMessages++; });

  for(int i = 0; i < numSendMessages; i++)
    send(EndPoint::TestEndPoint, msgToSend);

  test_detail::doMainLoop(1s, 1s, [&] { return numRecMessages <= numSendMessages; });
  c.disconnect();
}