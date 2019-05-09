#pragma once

#include <io/Bridge.h>
#include <tools/Throttler.h>
#include <list>

class FromEncoderBridge : public Bridge
{
 public:
  FromEncoderBridge();
  virtual ~FromEncoderBridge();

<<<<<<< HEAD
  void sendRotary(int8_t inc);

 private:
  void transmit(Receiver::tMessage msg) override;
=======
    void sendRotary(int8_t inc);

};
>>>>>>> layouts-reloaded

  void scheduleSimpleEvent(int8_t inc);
  void scheduleTimestampedEvent(int8_t inc);

  std::chrono::steady_clock::time_point m_firstPendingEventTime;
  static Domain getDomain();
};
