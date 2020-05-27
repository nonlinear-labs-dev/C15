#pragma once

class GenericScopeGuard
{
 public:
  template <typename tStart, typename tEnd>
  GenericScopeGuard(tStart start, tEnd end)
      : m_endCB { end }
  {
    start();
  }

  virtual ~GenericScopeGuard()
  {
    m_endCB();
  }

 private:
  std::function<void(void)> m_endCB;
};