#pragma once

#include "playground.h"

class RecursionGuard
{
  struct Lock
  {
  };

 public:
  typedef shared_ptr<Lock> tLock;
  shared_ptr<Lock> lock();

  bool isLocked() const;

 private:
  weak_ptr<Lock> m_lock;
};
