#pragma once

#include "playground.h"
#include "parameters/scale-converters/ScaleConverter.h"
#include <libundo/undo/Scope.h>

class RawValue
{
 public:
  typedef tControlPositionValue tValueType;

  RawValue(const ScaleConverter *scale, tValueType def);
  virtual ~RawValue();

  enum RawValueMeanings
  {
    CurrentState = 0,
    MorphA = 1,
    MorphB = 2
  };

  void undoableSetType(UNDO::Scope::tTransactionPtr transaction, PresetType oldType, PresetType desiredType);

  tValueType getRawValue() const;
  bool setRawValue(Initiator initiator, tValueType newRawValue);
  bool differs(tValueType other) const;
  void changeRawValue(Initiator initiator, tValueType diff);
  void setDefaultValue(tValueType defaultValue);
  tValueType getDefaultValue() const;
  bool isBiPolar() const;
  bool isBoolean() const;
  void setToDefault(Initiator initiator);
  virtual size_t getHash() const;

  const ScaleConverter *getScaleConverter() const;
  void setScaleConverter(const ScaleConverter *c);

  void setIsBoolean(bool v);

  virtual void onRawValueChanged(Initiator initiator, tValueType oldRawValue, tValueType newRawValue);

 private:
  RawValue(const RawValue &other) = delete;
  RawValue &operator=(const RawValue &) = delete;

  std::vector<tValueType> m_rawValue;
  tValueType m_defaultValue;
  const ScaleConverter *m_scaleConverter;
  bool m_isBoolean = false;
};
