#pragma once

#include "playground.h"
#include "proxies/hwui/HWUIEnums.h"
#include <tools/Throttler.h>
#include <chrono>
#include <io/network/WebSocketSession.h>

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
  sigc::connection onRotaryChanged(function<void(tIncrement)> slot);

  static void registerTests();

 private:
  void onMessage(WebSocketSession::tMessage msg);
  void open();
  tIncrement speedUp(tIncrement inc);
  void applyIncrement(tIncrement currentInc);

  Signal<void, tIncrement> m_signalRotaryChanged;
  Throttler m_throttler;
  int m_accumulatedIncs = 0;

  sigc::connection m_stress;
};
