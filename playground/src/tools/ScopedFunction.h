#pragma once
#include <memory>
#include <functional>

template <class CB> class ScopedFunction
{
 public:
  ScopedFunction(CB cb)
      : m_cb{ std::move(cb) }
  {
  }
  ~ScopedFunction()
  {
    m_cb();
  }

 protected:
  CB m_cb;
};