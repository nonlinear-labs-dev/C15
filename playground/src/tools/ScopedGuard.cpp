#include <tools/ScopedGuard.h>

ScopedGuard::ScopedGuard()
{
}

ScopedGuard::~ScopedGuard()
{
}

std::shared_ptr<ScopedGuard::Lock> ScopedGuard::lock()
{
  if(isLocked())
    return nullptr;

  m_currentLock = std::make_shared<ScopedGuard::Lock>();
  return m_currentLock;
}

bool ScopedGuard::isLocked() const
{
  return m_currentLock.lock() != nullptr;
}
