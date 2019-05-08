#pragma once

#include <playground.h>

class ScopedGuard
{
 public:
  ScopedGuard();
  virtual ~ScopedGuard();

  class Lock
  {
   public:
    Lock()
    {
    }
  };

  shared_ptr<Lock> lock();
  bool isLocked() const;

 private:
  weak_ptr<Lock> m_currentLock;
};
