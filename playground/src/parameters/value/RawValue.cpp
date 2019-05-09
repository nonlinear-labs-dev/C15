#include "RawValue.h"
#include <cmath>

RawValue::RawValue(const ScaleConverter *scale, tValueType def)
    : m_defaultValue(def)
    , m_factoryDefaultValue(def)
    , m_scaleConverter(scale)
{
  m_rawValue.resize(1, m_defaultValue);
}

RawValue::~RawValue()
{
}

const ScaleConverter *RawValue::getScaleConverter() const
{
  return m_scaleConverter;
}

void RawValue::setScaleConverter(const ScaleConverter *c)
{
  m_scaleConverter = c;
}

RawValue::tValueType RawValue::getRawValue() const
{
  return m_rawValue[0];
}

size_t RawValue::getHash() const
{
  size_t v = 0;

  for(auto &c : m_rawValue)
    hash_combine(v, c);

  return v;
}

bool RawValue::setRawValue(Initiator initiator, tValueType newRawValue)
{
  if(differs(newRawValue))
  {
    auto oldRawValue = m_rawValue[0];
    m_rawValue[0] = newRawValue;
    onRawValueChanged(initiator, oldRawValue, newRawValue);
    return true;
  }

  return false;
}

bool RawValue::differs(tValueType other) const
{
  return std::abs(other - m_rawValue[0]) > std::numeric_limits<tValueType>::epsilon();
}

void RawValue::changeRawValue(Initiator initiator, tValueType diff)
{
  setRawValue(initiator, m_rawValue[0] + diff);
}

RawValue::tValueType RawValue::getDefaultValue() const
{
  return m_defaultValue;
}

RawValue::tValueType RawValue::getFactoryDefaultValue() const
{
  return m_factoryDefaultValue;
}

void RawValue::setDefaultValue(tValueType defaultValue)
{
  m_defaultValue = defaultValue;
}

bool RawValue::isBiPolar() const
{
  return m_scaleConverter->isBiPolar();
}

void RawValue::setIsBoolean(bool v)
{
  m_isBoolean = v;
}

bool RawValue::isBoolean() const
{
  return m_isBoolean;
}

void RawValue::setToDefault(Initiator initiator)
{
  setRawValue(initiator, getDefaultValue());
}

void RawValue::onRawValueChanged(Initiator initiator, tValueType oldRawValue, tValueType newRawValue)
{
}
