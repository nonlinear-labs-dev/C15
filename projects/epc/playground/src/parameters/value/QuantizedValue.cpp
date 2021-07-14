#include "QuantizedValue.h"
#include <parameters/Parameter.h>
#include <Application.h>
#include <http/UndoScope.h>
#include <proxies/hwui/HWUI.h>
#include <algorithm>
#include <cmath>
#include <math.h>
#include <parameters/scale-converters/ScaleConverter.h>

size_t QuantizedValue::getHash() const
{
  std::hash<tValueType> h;
  return h(getQuantizedClipped());
}

QuantizedValue::IncrementalChanger::IncrementalChanger(Initiator initiator, QuantizedValue &value)
    : m_initiator(initiator)
    , m_value(value)
    , m_lastQuantizedValue(m_value.getQuantizedClipped())
    , m_pendingAmount(0)
{
  m_value.setRawValue(m_initiator, m_lastQuantizedValue);
}

QuantizedValue::IncrementalChanger::IncrementalChanger(const IncrementalChanger &other)
    : m_value(other.m_value)
    , m_lastQuantizedValue(other.m_lastQuantizedValue)
    , m_pendingAmount(other.m_pendingAmount)
{
}

bool QuantizedValue::IncrementalChanger::isManaging(const QuantizedValue &v) const
{
  return &v == &m_value;
}

void QuantizedValue::IncrementalChanger::changeBy(UNDO::Transaction *transaction, tControlPositionValue amount,
                                                  bool fine)
{
  if(fine)
    amount = amount * m_value.getCoarseDenominator() / m_value.getFineDenominator();

  m_pendingAmount += amount;

  auto newVal = m_value.getQuantizedValue(m_lastQuantizedValue + m_pendingAmount, fine);

  if(newVal != m_lastQuantizedValue && m_value.m_owner != nullptr)
  {
    newVal = m_value.clip(newVal);

    if(m_value.isBoolean())
    {
      if(newVal > m_lastQuantizedValue)
        newVal = 1.0;
      else if(newVal < m_lastQuantizedValue)
        newVal = 0.0;
    }

    auto owner = m_value.m_owner;
    owner->setCPFromHwui(transaction, newVal);

    m_pendingAmount = 0;
    m_lastQuantizedValue = newVal;
  }
}

Parameter *QuantizedValue::IncrementalChanger::getOwner()
{
  return m_value.m_owner;
}

QuantizedValue::QuantizedValue(Parameter *owner, const ScaleConverter *scale, tControlPositionValue def,
                               tControlPositionValue coarseDenominator, tControlPositionValue fineDenominator)
    : super(scale, def)
    , m_owner(owner)
    , m_coarseDenominator(coarseDenominator)
    , m_fineDenominator(fineDenominator)
{
}

QuantizedValue::~QuantizedValue()
{
}

tTcdValue QuantizedValue::getTcdValue() const
{
  return getScaleConverter()->controlPositionToTcd(getQuantizedClipped());
}

void QuantizedValue::setTcdValue(tTcdValue v)
{
  auto cp = getScaleConverter()->tcdToControlPosition(v);
  setRawValue(Initiator::INDIRECT, cp);
}

tDisplayValue QuantizedValue::getDisplayValue() const
{
  return getScaleConverter()->controlPositionToDisplay(getQuantizedClipped());
}

Glib::ustring QuantizedValue::getDisplayString() const
{
  return getScaleConverter()->getDimension().stringize(getDisplayValue());
}

Glib::ustring QuantizedValue::getDisplayString(tControlPositionValue cp) const
{
  return getScaleConverter()->getDimension().stringize(getScaleConverter()->controlPositionToDisplay(cp));
}

tControlPositionValue QuantizedValue::getNextStepValue(int incs, ButtonModifiers modifiers) const
{
  return getNextStepValue(getRawValue(), incs, modifiers);
}

tControlPositionValue QuantizedValue::getNextStepValue(int incs, bool fine, bool shift) const
{
  return getNextStepValue(getRawValue(), incs, fine, shift);
}

tControlPositionValue QuantizedValue::getNextStepValue(tControlPositionValue value, int incs, bool fine,
                                                       bool shift) const
{
  if(shift || isBoolean())
  {
    if(incs > 0)
    {
      return getScaleConverter()->getControlPositionRange().getMax();
    }
    else if(incs < 0)
    {
      if(isBoolean())
        return 0;

      return getScaleConverter()->getControlPositionRange().getMin();
    }
  }

  if(!fine && !isValueCoarseQuantized())
  {
    double fineValue = getQuantizedClippedValue(true);
    double coarseValue = getQuantizedClippedValue(false);

    if(coarseValue < fineValue && incs == -1)
    {
      incs = 0;
    }
    else if(coarseValue > fineValue && incs == 1)
    {
      incs = 0;
    }
  }

  auto controlVal = getClippedValue(value);
  auto denominator = fine ? m_fineDenominator : m_coarseDenominator;
  auto unRounded = controlVal * denominator;
  auto rounded = std::round(unRounded);
  auto newValue = clip((rounded + incs) / denominator);
  return newValue;
}

tControlPositionValue QuantizedValue::getNextStepValue(tControlPositionValue value, int incs,
                                                       ButtonModifiers modifiers) const
{
  bool fine = modifiers[ButtonModifier::FINE];
  bool shift = modifiers[ButtonModifier::SHIFT];
  return getNextStepValue(value, incs, fine, shift);
}

bool QuantizedValue::isValueCoarseQuantized() const
{
  auto fine = getQuantizedClippedValue(true);
  auto coarse = getQuantizedClippedValue(false);
  return fine == coarse;
}

tControlPositionValue QuantizedValue::getQuantizedClipped() const
{
  return getQuantizedClippedValue(true);
}

tControlPositionValue QuantizedValue::getCoarseDenominator() const
{
  return m_coarseDenominator;
}

void QuantizedValue::setCoarseDenominator(tControlPositionValue coarseDenominator)
{
  m_coarseDenominator = coarseDenominator;
}

tControlPositionValue QuantizedValue::getFineDenominator() const
{
  return m_fineDenominator;
}

void QuantizedValue::setFineDenominator(tControlPositionValue fineDenominator)
{
  m_fineDenominator = fineDenominator;
}

tControlPositionValue QuantizedValue::getFineQuantizedClippedValue(tControlPositionValue newValue) const
{
  return getQuantizedValue(clip(newValue), true);
}

tControlPositionValue QuantizedValue::getQuantizedClippedValue(bool fine) const
{
  return getQuantizedValue(getClippedValue(), fine);
}

tControlPositionValue QuantizedValue::getQuantizedUnClippedValue(bool fine) const
{
  return getQuantizedValue(getRawValue(), fine);
}

tControlPositionValue QuantizedValue::getQuantizedValue(tControlPositionValue v, bool fine) const
{
  auto steps = fine ? m_fineDenominator : m_coarseDenominator;
  v *= steps;
  v = std::round(v);
  return v / steps;
}

void QuantizedValue::resetSaturation()
{
  setRawValue(Initiator::INDIRECT, getQuantizedClipped());
}

void QuantizedValue::inc(Initiator initiator, ButtonModifiers modifiers)
{
  bool fine = modifiers[ButtonModifier::FINE];
  int denominator = fine ? m_fineDenominator : m_coarseDenominator;
  auto v = std::round(getQuantizedClippedValue(fine) * denominator);
  v++;
  setRawValue(initiator, v / denominator);
}

void QuantizedValue::dec(Initiator initiator, ButtonModifiers modifiers)
{
  bool fine = modifiers[ButtonModifier::FINE];
  int denominator = fine ? m_fineDenominator : m_coarseDenominator;
  auto v = std::round(getQuantizedClippedValue(fine) * denominator);
  v--;
  setRawValue(initiator, v / denominator);
}

void QuantizedValue::applyModulation(tControlPositionValue delta)
{
  setRawValue(Initiator::INDIRECT, getRawValue() + delta);
}

std::unique_ptr<QuantizedValue::IncrementalChanger> QuantizedValue::startUserEdit(Initiator initiator)
{
  return std::unique_ptr<QuantizedValue::IncrementalChanger>(new IncrementalChanger(initiator, *this));
}

void QuantizedValue::onRawValueChanged(Initiator initiator, tValueType oldRawValue, tValueType newRawValue)
{
  if(m_owner)
    m_owner->onValueChanged(initiator, oldRawValue, newRawValue);

  super::onRawValueChanged(initiator, oldRawValue, newRawValue);
}

void QuantizedValue::onClippedValueChanged(Initiator initiator, tControlPositionValue oldClippedValue,
                                           tControlPositionValue newClippedValue)
{
  auto oldFine = getQuantizedValue(oldClippedValue, true);
  auto newFine = getQuantizedValue(newClippedValue, true);

  if(oldFine != newFine)
    onFineQuantizedChanged(initiator, oldFine, newFine);
}
void QuantizedValue::onFineQuantizedChanged(Initiator initiator, tControlPositionValue oldFine,
                                            tControlPositionValue newFine)
{
  if(m_owner)
    m_owner->onValueFineQuantizedChanged(initiator, oldFine, newFine);
}
