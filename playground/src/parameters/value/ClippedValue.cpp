#include "ClippedValue.h"
#include <algorithm>

ClippedValue::ClippedValue(const ScaleConverter *scale, RawValue::tValueType def)
    : super(scale, def)
{
}

ClippedValue::~ClippedValue()
{
}

RawValue::tValueType ClippedValue::getClippedValue() const
{
  return clip(getRawValue());
}

RawValue::tValueType ClippedValue::clip(RawValue::tValueType v) const
{
  return std::min(getUpperBorder(), std::max(v, getLowerBorder()));
}

RawValue::tValueType ClippedValue::getUpperBorder() const
{
  return 1.0;
}

RawValue::tValueType ClippedValue::getLowerBorder() const
{
  return isBiPolar() ? -1.0 : 0.0;
}

void ClippedValue::onRawValueChanged(Initiator initiator, RawValue::tValueType oldRawValue,
                                     RawValue::tValueType newRawValue)
{
  auto oldClippedValue = clip(oldRawValue);
  auto newClippedValue = clip(newRawValue);

  if(oldClippedValue != newClippedValue)
    onClippedValueChanged(initiator, oldClippedValue, newClippedValue);

  super::onRawValueChanged(initiator, oldRawValue, newRawValue);
}