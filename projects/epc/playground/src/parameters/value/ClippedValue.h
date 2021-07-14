#pragma once

#include "RawValue.h"

class ClippedValue : public RawValue
{
 private:
  typedef RawValue super;

 public:
  ClippedValue(const ScaleConverter* scale, tValueType def);
  ~ClippedValue() override;

  [[nodiscard]] tValueType getClippedValue() const;
  [[nodiscard]] tValueType getClippedValue(tValueType t) const;
  [[nodiscard]] tValueType clip(tValueType v) const;
  [[nodiscard]] tValueType getUpperBorder() const;
  [[nodiscard]] tValueType getLowerBorder() const;
  ClippedValue(const ClippedValue& other) = delete;
  ClippedValue& operator=(const ClippedValue&) = delete;

 protected:
  void onRawValueChanged(Initiator initiator, tValueType oldRawValue, tValueType newRawValue) override;
  virtual void onClippedValueChanged(Initiator initiator, tValueType oldClippedValue, tValueType newClippedValue) = 0;
};
