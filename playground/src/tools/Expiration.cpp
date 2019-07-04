#include <utility>

#include "Expiration.h"

Expiration::Expiration(Expiration::Callback cb, Expiration::Duration d, int priority)
{
  setCallback(std::move(cb));

  if(d > Duration::zero())
    refresh(d, priority);
}

Expiration::~Expiration()
{
  m_timeout.disconnect();
}

void Expiration::setCallback(Expiration::Callback cb)
{
  m_cb = std::move(cb);

  if(isPending() && !m_cb)
    cancel();
}

bool Expiration::isPending() const
{
  return m_timeout.connected();
}

void Expiration::cancel()
{
  m_timeout.disconnect();
}

void Expiration::refresh(Expiration::Duration d, int priority)
{
  cancel();

  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
  m_timeout = Glib::MainContext::get_default()->signal_timeout().connect(mem_fun(this, &Expiration::doCallback), ms,
                                                                         priority);
}

bool Expiration::doCallback()
{
  cancel();

  if(auto cb = m_cb)
    cb();

  return false;
}
