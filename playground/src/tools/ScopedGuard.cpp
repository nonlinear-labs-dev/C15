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

  auto ret = std::make_shared<ScopedGuard::Lock>();
  m_currentLock = ret;
  return ret;
}

bool ScopedGuard::isLocked() const
{
  return m_currentLock.lock() != nullptr;
}
