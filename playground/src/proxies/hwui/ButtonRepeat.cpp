#include "ButtonRepeat.h"
#include "Application.h"
#include <device-settings/DebugLevel.h>

ButtonRepeat::ButtonRepeat(function<void()> cb)
{
  cb();

  m_buttonRepeat.disconnect();
  m_buttonRepeat = Application::get().getMainContext()->signal_timeout().connect(
      sigc::bind(sigc::mem_fun(this, &ButtonRepeat::onButtonRepeatInitialTimeoutElapsed), cb), 500);
}

ButtonRepeat::~ButtonRepeat()
{
}

bool ButtonRepeat::onButtonRepeatInitialTimeoutElapsed(function<void()> cb)
{
  cb();

  m_buttonRepeat.disconnect();
  m_buttonRepeat = Application::get().getMainContext()->signal_timeout().connect(
      sigc::bind(sigc::mem_fun(this, &ButtonRepeat::onButtonRepeatRegularTimeoutElapsed), cb), 120);

  return false;
}

bool ButtonRepeat::onButtonRepeatRegularTimeoutElapsed(function<void()> cb)
{
  cb();
  return true;
}
