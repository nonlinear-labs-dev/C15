#pragma once

#include <nltools/threading/Expiration.h>
#include <functional>

class ShortVsLongPress
{
 public:
  using Callback = std::function<void()>;
  ShortVsLongPress(Callback shortPressAction, Callback longPressAction);

  void onButtonEvent(bool state);

 private:
  void onButtonDown();
  void onButtonUp();
  void expired();

  Callback m_shortPressAction;
  Callback m_longPressAction;
  Expiration m_exp;
};
