#include "Hysteresis.h"

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
