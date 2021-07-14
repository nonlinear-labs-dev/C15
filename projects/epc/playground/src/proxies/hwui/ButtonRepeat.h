#pragma once

#include "playground.h"
#include <sigc++/trackable.h>
#include <sigc++/connection.h>
#include <functional>

class ButtonRepeat : public sigc::trackable
{
 public:
  using Callback = std::function<void()>;
  explicit ButtonRepeat(const Callback& cb);
  ~ButtonRepeat();

 private:
  bool onTimeoutElapsed(const Callback& cb);
  void installRepeat(const Callback& cb, uint ms);

  sigc::connection m_buttonRepeat;
};
