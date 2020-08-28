#include "RawValue.h"
#include <parameters/scale-converters/ScaleConverter.h>
#include <cmath>

RawValue::RawValue(const ScaleConverter *scale, tValueType def)
    : m_defaultValue(def)
    , m_rawValue(def)
    , m_factoryDefaultValue(def)
    , m_scaleConverter(scale)
{
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
  return m_rawValue;
}

size_t RawValue::getHash() const
{
  size_t v = 0;
  hash_combine(v, m_rawValue);
  return v;
}

bool RawValue::setRawValue(Initiator initiator, tValueType newRawValue)
{
  if(differs(newRawValue))
  {
    onRawValueChanged(initiator, std::exchange(m_rawValue, newRawValue), newRawValue);
    return true;
  }

  return false;
}

bool RawValue::differs(tValueType other) const
{
  return std::abs(other - m_rawValue) > std::numeric_limits<tValueType>::epsilon();
}

void RawValue::changeRawValue(Initiator initiator, tValueType diff)
{
  setRawValue(initiator, m_rawValue + diff);
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

bool RawValue::setIsBoolean(bool v)
{
  auto old = std::exchange(m_isBoolean, v);
  return old != v;
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
