#include "ButtonRepeat.h"
#include "Application.h"
#include <nltools/logging/Log.h>
#include <glibmm/main.h>

ButtonRepeat::ButtonRepeat(Callback cb)
{
  installRepeat(cb, 500);
}

ButtonRepeat::~ButtonRepeat()
{
  nltools::Log::warning("ButtonRepeat destroyed");
}

void ButtonRepeat::installRepeat(Callback cb, uint ms)
{
  cb();
  m_buttonRepeat.disconnect();
  m_buttonRepeat = Application::get().getMainContext()->signal_timeout().connect(
      sigc::bind(sigc::mem_fun(this, &ButtonRepeat::onTimeoutElapsed), cb), ms);
}

bool ButtonRepeat::onTimeoutElapsed(Callback cb)
{
  nltools::Log::warning("ButtonRepeat elapsed");
  installRepeat(cb, 120);
  return false;
}
