#pragma once

#include "LED.h"
#include <sigc++/connection.h>

class Application;

class TwoStateLED : public LED
{
 public:
  explicit TwoStateLED(int id);
  ~TwoStateLED() override;

  enum LedState
  {
    OFF,
    ON,
    BLINK
  };

  void setState(LedState state);
  LedState getState();
  void init();
  void syncBBBB();

 private:
  void switchLED(bool onOrOff);
  void onBlinkUpdate(int blinkCount);

  LedState m_state;
  sigc::connection m_blinkTimer;
};
