#pragma once

#include "playground.h"
#include "proxies/hwui/HWUIEnums.h"
#include <nltools/threading/Throttler.h>
#include <tools/Signal.h>

namespace nltools
{
  namespace msg
  {
    class RotaryChangedMessage;
    class TimestampedRotaryChangedMessage;
  }
}

class RotaryEncoder
{
 public:
  class Receiver
  {
   public:
    virtual bool onRotary(int inc, ButtonModifiers modifiers) = 0;
  };

  typedef signed char tIncrement;

  RotaryEncoder();
  virtual ~RotaryEncoder();

  void fake(tIncrement amount);
  sigc::connection onRotaryChanged(sigc::slot<void, tIncrement> slot);
  int64_t resetOldestPendingTimestamp();

 private:
  void onMessage(const nltools::msg::RotaryChangedMessage &msg);
  void onMessageTimestamped(const nltools::msg::TimestampedRotaryChangedMessage &msg);

  void applyIncrement(tIncrement currentInc);

  Signal<void, tIncrement> m_signalRotaryChanged;
  Throttler m_throttler;
  int m_accumulatedIncs = 0;
  uint64_t m_oldestPendingTimestamp = 0;

  sigc::connection m_stress;
};
