#pragma once

#include "ClippedValue.h"
#include "proxies/hwui/HWUIEnums.h"
#include <memory>
#include <libundo/undo/Transaction.h>

class Parameter;

class QuantizedValue : public ClippedValue
{
 private:
  typedef ClippedValue super;

 public:
  QuantizedValue(Parameter *owner, const ScaleConverter *scale, tValueType def, tControlPositionValue coarseDenominator,
                 tControlPositionValue fineDenominator);
  virtual ~QuantizedValue();

  class IncrementalChanger
  {
   public:
    IncrementalChanger(Initiator initiator, QuantizedValue &value);
    IncrementalChanger(const IncrementalChanger &other);
    void changeBy(UNDO::Transaction* transaction, tControlPositionValue amount, bool fine);

    bool isManaging(const QuantizedValue &v) const;

   Parameter* getOwner();private:
    Initiator m_initiator;
    QuantizedValue &m_value;
    tValueType m_lastQuantizedValue;
    tValueType m_pendingAmount;
  };

  tControlPositionValue getQuantizedClipped() const;
  tControlPositionValue getCoarseDenominator() const;
  void setCoarseDenominator(tControlPositionValue coarseDenominator);
  tControlPositionValue getFineDenominator() const;

  void setFineDenominator(tControlPositionValue fineDenominator);

  tControlPositionValue getQuantizedClippedValue(bool fine) const;
  tControlPositionValue getQuantizedUnClippedValue(bool fine) const;

  tControlPositionValue getQuantizedValue(tControlPositionValue v, bool fine) const;

  void inc(Initiator initiator, ButtonModifiers modifiers);
  void dec(Initiator initiator, ButtonModifiers modifiers);
  void resetSaturation();
  void applyModulation(tControlPositionValue delta);

  tControlPositionValue getNextStepValue(int incs, ButtonModifiers modifiers) const;
  tControlPositionValue getNextStepValue(int incs, bool fine, bool shift) const;
  tControlPositionValue getNextStepValue(tControlPositionValue value, int incs, ButtonModifiers modifiers) const;
  tControlPositionValue getNextStepValue(tControlPositionValue value, int incs, bool fine, bool shift) const;

  tTcdValue getTcdValue() const;
  void setTcdValue(tTcdValue v);
  tDisplayValue getDisplayValue() const;
  Glib::ustring getDisplayString() const;
  Glib::ustring getDisplayString(tControlPositionValue cp) const;
  virtual size_t getHash() const override;

  std::unique_ptr<IncrementalChanger> startUserEdit(Initiator initiator);

  virtual void onClippedValueChanged(Initiator initiator, tControlPositionValue oldClippedValue,
                                     tControlPositionValue newClippedValue) override;
  virtual void onFineQuantizedChanged(Initiator initiator, tControlPositionValue oldFine,
                                      tControlPositionValue newFine);
  virtual void onRawValueChanged(Initiator initiator, tValueType oldRawValue, tValueType newRawValue) override;

 private:
  QuantizedValue(const QuantizedValue &other) = delete;
  QuantizedValue &operator=(const QuantizedValue &) = delete;
  bool isValueCoarseQuantized() const;

  Parameter *m_owner;

  tControlPositionValue m_coarseDenominator;
  tControlPositionValue m_fineDenominator;

  friend class IncrementalChanger;
};
