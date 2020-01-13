#pragma once

#include <io/Bridge.h>
#include <nltools/messaging/Message.h>

namespace nltools
{
  namespace msg
  {
    struct SetOLEDMessage;
    struct SetTimestampedOledMessage;
  }
}

class ToOledsBridge : public Bridge
{
  using super = Bridge;

 public:
  ToOledsBridge();

 private:
  bool printTurnaroundTime();
  void removeOldRecords();
  void onTimestampedMessageRecieved(const nltools::msg::SetTimestampedOledMessage &msg);
  void onMessageReceived(const nltools::msg::SetOLEDMessage &msg);

  struct Record
  {
    std::chrono::steady_clock::time_point captureTime;
    int64_t ms;
  };

  std::list<Record> m_timeStamps;
};
