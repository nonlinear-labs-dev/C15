#pragma once

#include "playground.h"

class ScaleConverter;

class RawValue
{
 public:
  typedef tControlPositionValue tValueType;

  RawValue(const ScaleConverter *scale, tValueType def);
  RawValue(const RawValue &other) = delete;
  RawValue &operator=(const RawValue &) = delete;

  virtual ~RawValue();

  [[nodiscard]] tValueType getRawValue() const;
  bool setRawValue(Initiator initiator, tValueType newRawValue);
  [[nodiscard]] bool differs(tValueType other) const;
  [[nodiscard]] bool equals(tValueType other) const;
  void changeRawValue(Initiator initiator, tValueType diff);
  void setDefaultValue(tValueType defaultValue);
  [[nodiscard]] tValueType getDefaultValue() const;
  [[nodiscard]] tValueType getFactoryDefaultValue() const;

  [[nodiscard]] bool isBiPolar() const;
  [[nodiscard]] bool isBoolean() const;
  void setToDefault(Initiator initiator);
  [[nodiscard]] virtual size_t getHash() const;

  [[nodiscard]] const ScaleConverter *getScaleConverter() const;
  void setScaleConverter(const ScaleConverter *c);

  bool setIsBoolean(bool v);

  virtual void onRawValueChanged(Initiator initiator, tValueType oldRawValue, tValueType newRawValue);

 private:
  tValueType m_defaultValue;
  tValueType m_rawValue;
  tValueType m_factoryDefaultValue;

  const ScaleConverter *m_scaleConverter;
  bool m_isBoolean = false;
};
