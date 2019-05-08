#pragma once

#include "RawValue.h"

class ClippedValue : public RawValue
{
 private:
  typedef RawValue super;

 public:
  ClippedValue(const ScaleConverter* scale, tValueType def);
  virtual ~ClippedValue();

  tValueType getClippedValue() const;
  tValueType clip(tValueType v) const;
  tValueType getUpperBorder() const;
  tValueType getLowerBorder() const;

 protected:
  void onRawValueChanged(Initiator initiator, tValueType oldRawValue, tValueType newRawValue) override;
  virtual void onClippedValueChanged(Initiator initiator, tValueType oldClippedValue, tValueType newClippedValue) = 0;

 private:
  ClippedValue(const ClippedValue& other) = delete;
  ClippedValue& operator=(const ClippedValue&) = delete;
};
