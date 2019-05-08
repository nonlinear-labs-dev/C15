#pragma once

#include "playground.h"

class ButtonRepeat : public sigc::trackable
{
 public:
  ButtonRepeat(function<void()> cb);
  ~ButtonRepeat();

 private:
  bool onButtonRepeatInitialTimeoutElapsed(function<void()> cb);
  bool onButtonRepeatRegularTimeoutElapsed(function<void()> cb);

  sigc::connection m_buttonRepeat;
};
