#include <tools/ScopedGuard.h>

ScopedGuard::ScopedGuard()
{
}

ScopedGuard::~ScopedGuard()
{
}

shared_ptr<ScopedGuard::Lock> ScopedGuard::lock()
{
  if(isLocked())
    return nullptr;

  shared_ptr<ScopedGuard::Lock> ptr(new Lock());
  m_currentLock = ptr;
  return ptr;
}

bool ScopedGuard::isLocked() const
{
  return m_currentLock.lock() != nullptr;
}
