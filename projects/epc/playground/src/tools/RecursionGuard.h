#pragma once

#include "playground.h"
#include <memory>

class RecursionGuard
{
  struct Lock
  {
  };

 public:
  typedef std::shared_ptr<Lock> tLock;
  std::shared_ptr<Lock> lock();

  [[nodiscard]] bool isLocked() const;

 private:
  std::weak_ptr<Lock> m_lock;
};
