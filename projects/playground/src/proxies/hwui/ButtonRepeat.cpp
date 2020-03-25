#include "ButtonRepeat.h"
#include "Application.h"
#include <nltools/logging/Log.h>
#include <glibmm/main.h>

ButtonRepeat::ButtonRepeat(const Callback& cb)
{
  installRepeat(cb, 500);
}

ButtonRepeat::~ButtonRepeat() = default;

void ButtonRepeat::installRepeat(const Callback& cb, uint ms)
{
  cb();
  m_buttonRepeat.disconnect();
  m_buttonRepeat = Application::get().getMainContext()->signal_timeout().connect(
      sigc::bind(sigc::mem_fun(this, &ButtonRepeat::onTimeoutElapsed), cb), ms);
}

bool ButtonRepeat::onTimeoutElapsed(const Callback& cb)
{
  installRepeat(cb, 120);
  return false;
}
