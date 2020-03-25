#include "Signal.h"
#include <sigc++/trackable.h>
#include <glibmm/main.h>

SignalBase::SignalBase()
    : m_initCallbackScheduled(false)
{
}

SignalBase::~SignalBase()
{
  m_initCallbackConnection.disconnect();
  m_deferedCallbackConnection.disconnect();
}

void SignalBase::scheduleInitCallback(const void *cookie, SignalBase::tCallback &&cb)
{
  m_initRecords.push_back({ std::move(cb), cookie });

  if(!m_initCallbackScheduled.exchange(true))
  {
    m_initCallbackConnection = Glib::MainContext::get_default()->signal_idle().connect(
        sigc::mem_fun(this, &SignalBase::doTheCallbacks), Glib::PRIORITY_HIGH);
  }
}

bool SignalBase::doTheCallbacks()
{
  m_initCallbackScheduled.exchange(false);

  tInitRecords initRecords;
  swap(initRecords, m_initRecords);

  m_initCallbackConnection.disconnect();

  for(const auto &r : initRecords)
    if(findCookie(r.cookie))
      r.cb();

  return false;
}

void SignalBase::deferedSend()
{
  if(!m_deferedCallbackConnection.connected())
  {
    auto ctx = Glib::MainContext::get_default();
    m_deferedCallbackConnection
        = ctx->signal_idle().connect(sigc::mem_fun(this, &SignalBase::emitDefered), Glib::PRIORITY_DEFAULT);
  }
}

bool SignalBase::emitDefered()
{
  m_deferedCallbackConnection.disconnect();

  tCallback c;
  std::swap(c, m_deferedSend);

  if(c)
    c();

  return false;
}
