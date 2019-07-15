#pragma once
#include <atomic>
#include <glibmm/ustring.h>

class ConditionBase
{
 public:
  ConditionBase()
  {
    m_cache = false;
  }

  bool get() const
  {
    auto res = check();
    if(m_cache != res)
    {
      m_cache = res;
      onConditionChanged();
    }
    return res;
  }

 protected:
  virtual void onConditionChanged() const = 0;
  virtual bool check() const = 0;
  mutable std::atomic_bool m_cache;
};
