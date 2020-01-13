#include "ButtonRepeat.h"
#include "Application.h"
#include <device-settings/DebugLevel.h>

ButtonRepeat::ButtonRepeat(std::function<void()> cb)
{
  cb();

#warning "adlerauge"
  m_buttonRepeat.disconnect();
  m_buttonRepeat = Application::get().getMainContext()->signal_timeout().connect(
      sigc::bind(sigc::mem_fun(this, &ButtonRepeat::onButtonRepeatInitialTimeoutElapsed), cb), 800);
}

ButtonRepeat::~ButtonRepeat()
{
}

bool ButtonRepeat::onButtonRepeatInitialTimeoutElapsed(std::function<void()> cb)
{
  cb();

  m_buttonRepeat.disconnect();
  m_buttonRepeat = Application::get().getMainContext()->signal_timeout().connect(
      sigc::bind(sigc::mem_fun(this, &ButtonRepeat::onButtonRepeatRegularTimeoutElapsed), cb), 120);

  return false;
}

bool ButtonRepeat::onButtonRepeatRegularTimeoutElapsed(std::function<void()> cb)
{
  cb();
  return true;
}
