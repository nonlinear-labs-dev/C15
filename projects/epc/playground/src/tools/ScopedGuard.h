#pragma once

#include <memory>

class ScopedGuard
{
 public:
  ScopedGuard();
  virtual ~ScopedGuard();

  class Lock
  {
   public:
    Lock() = default;
  };

  std::shared_ptr<Lock> lock();
  [[nodiscard]] bool isLocked() const;

 private:
  std::weak_ptr<Lock> m_currentLock;
};
