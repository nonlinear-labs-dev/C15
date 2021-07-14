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

  QuantizedValue(const QuantizedValue &other) = delete;
  QuantizedValue &operator=(const QuantizedValue &) = delete;

  ~QuantizedValue() override;

  class IncrementalChanger
  {
   public:
    IncrementalChanger(Initiator initiator, QuantizedValue &value);
    IncrementalChanger(const IncrementalChanger &other);
    void changeBy(UNDO::Transaction *transaction, tControlPositionValue amount, bool fine);

    [[nodiscard]] bool isManaging(const QuantizedValue &v) const;

    Parameter *getOwner();

   private:
    Initiator m_initiator;
    QuantizedValue &m_value;
    tValueType m_lastQuantizedValue;
    tValueType m_pendingAmount;
  };

  [[nodiscard]] tControlPositionValue getQuantizedClipped() const;
  [[nodiscard]] tControlPositionValue getCoarseDenominator() const;
  void setCoarseDenominator(tControlPositionValue coarseDenominator);
  [[nodiscard]] tControlPositionValue getFineDenominator() const;

  void setFineDenominator(tControlPositionValue fineDenominator);

  [[nodiscard]] tControlPositionValue getFineQuantizedClippedValue(tControlPositionValue newValue) const;
  [[nodiscard]] tControlPositionValue getQuantizedClippedValue(bool fine) const;
  [[nodiscard]] tControlPositionValue getQuantizedUnClippedValue(bool fine) const;

  [[nodiscard]] tControlPositionValue getQuantizedValue(tControlPositionValue v, bool fine) const;

  void inc(Initiator initiator, ButtonModifiers modifiers);
  void dec(Initiator initiator, ButtonModifiers modifiers);
  void resetSaturation();
  void applyModulation(tControlPositionValue delta);

  [[nodiscard]] tControlPositionValue getNextStepValue(int incs, ButtonModifiers modifiers) const;
  [[nodiscard]] tControlPositionValue getNextStepValue(int incs, bool fine, bool shift) const;
  [[nodiscard]] tControlPositionValue getNextStepValue(tControlPositionValue value, int incs,
                                                       ButtonModifiers modifiers) const;
  [[nodiscard]] tControlPositionValue getNextStepValue(tControlPositionValue value, int incs, bool fine,
                                                       bool shift) const;

  [[nodiscard]] tTcdValue getTcdValue() const;
  void setTcdValue(tTcdValue v);
  [[nodiscard]] tDisplayValue getDisplayValue() const;
  [[nodiscard]] Glib::ustring getDisplayString() const;
  [[nodiscard]] Glib::ustring getDisplayString(tControlPositionValue cp) const;
  [[nodiscard]] size_t getHash() const override;

  std::unique_ptr<IncrementalChanger> startUserEdit(Initiator initiator);

  void onClippedValueChanged(Initiator initiator, tControlPositionValue oldClippedValue,
                             tControlPositionValue newClippedValue) override;
  virtual void onFineQuantizedChanged(Initiator initiator, tControlPositionValue oldFine,
                                      tControlPositionValue newFine);
  void onRawValueChanged(Initiator initiator, tValueType oldRawValue, tValueType newRawValue) override;

 private:
  [[nodiscard]] bool isValueCoarseQuantized() const;

  Parameter *m_owner;

  tControlPositionValue m_coarseDenominator;
  tControlPositionValue m_fineDenominator;

  friend class IncrementalChanger;
};
