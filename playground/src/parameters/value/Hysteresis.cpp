#include "Hysteresis.h"
#include <testing/TestDriver.h>

TestDriver<Hysteresis> tests;

Hysteresis::Hysteresis(double initVal, double hyst)
    : m_value(initVal)
    , m_hyst(hyst)
    , m_rangeMin(initVal - hyst / 2)
    , m_rangeMax(initVal + hyst / 2)
    , m_reportedValue(initVal)
{
}

double Hysteresis::set(double v)
{
  return applyChange(v - m_value);
}

double Hysteresis::applyChange(double v)
{
  m_value += v;

  if(m_value >= m_rangeMax)
  {
    m_rangeMin = m_value - m_hyst;
    m_rangeMax = m_value;
    m_reportedValue = m_value;
  }
  else if(m_value <= m_rangeMin)
  {
    m_rangeMin = m_value;
    m_rangeMax = m_value + m_hyst;
    m_reportedValue = m_value;
  }

  return m_reportedValue;
}

void Hysteresis::registerTests()
{
  g_test_add_func("/Hysteresis/set", []() {
    Hysteresis h(0.0, 0.1);
    g_assert(h.set(0.01) == 0.0);
    g_assert(h.set(-0.01) == 0.0);
    g_assert(h.set(0.05) == 0.05);
    g_assert(h.set(-0.04) == 0.05);
    g_assert(h.set(-0.05) == -0.05);
    g_assert(h.set(-0.125) == -0.125);
    g_assert(h.set(-0.15) == -0.15);
    g_assert(h.set(-0.125) == -0.15);
  });
}
