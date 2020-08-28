#pragma once

#include "playground.h"

class ScaleConverter;

class RawValue
{
 public:
  typedef tControlPositionValue tValueType;

  RawValue(const ScaleConverter *scale, tValueType def);
  virtual ~RawValue();

  tValueType getRawValue() const;
  bool setRawValue(Initiator initiator, tValueType newRawValue);
  bool differs(tValueType other) const;
  void changeRawValue(Initiator initiator, tValueType diff);
  void setDefaultValue(tValueType defaultValue);
  tValueType getDefaultValue() const;
  tValueType getFactoryDefaultValue() const;

  bool isBiPolar() const;
  bool isBoolean() const;
  void setToDefault(Initiator initiator);
  virtual size_t getHash() const;

  const ScaleConverter *getScaleConverter() const;
  void setScaleConverter(const ScaleConverter *c);

  bool setIsBoolean(bool v);

  virtual void onRawValueChanged(Initiator initiator, tValueType oldRawValue, tValueType newRawValue);

 private:
  RawValue(const RawValue &other) = delete;
  RawValue &operator=(const RawValue &) = delete;

  tValueType m_defaultValue;
  tValueType m_rawValue;
  tValueType m_factoryDefaultValue;

  const ScaleConverter *m_scaleConverter;
  bool m_isBoolean = false;
};
