#pragma once

#include "playground.h"
#include <sigc++/trackable.h>
#include <sigc++/connection.h>
#include <functional>

class ButtonRepeat : public sigc::trackable
{
 public:
  using Callback = std::function<void()>;
  ButtonRepeat(Callback cb);
  ~ButtonRepeat();

 private:
  bool onTimeoutElapsed(Callback cb);
  void installRepeat(Callback cb, uint ms);

  sigc::connection m_buttonRepeat;
};
