#pragma once

#include "PhysicalControlParameter.h"

class PedalType;

class PedalParameter : public PhysicalControlParameter
{
 private:
  typedef PhysicalControlParameter super;

 public:
  using super::super;

  enum PedalModes
  {
    STAY = 0,
    RETURN_TO_ZERO = 1,
    RETURN_TO_CENTER = 2,
    NUM_PEDAL_MODES
  };

  void undoableSetPedalMode(UNDO::Scope::tTransactionPtr transaction, PedalModes mode);
  void undoableSetPedalMode(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &mode);
  void undoableIncPedalMode(UNDO::Scope::tTransactionPtr transaction);
  PedalModes getPedalMode() const;
  virtual ReturnMode getReturnMode() const override;
  virtual void copyFrom(UNDO::Scope::tTransactionPtr transaction, Parameter *other) override;
  virtual void loadDefault(UNDO::Scope::tTransactionPtr transaction) override;

  shared_ptr<PedalType> getAssociatedPedalTypeSetting() const;

 protected:
  virtual void writeDocProperties(Writer &writer, tUpdateID knownRevision) const override;
  void writeDifferences(Writer &writer, Parameter *other) const override;
  virtual void onPresetSentToLpc() const override;
  virtual bool shouldWriteDocProperties(tUpdateID knownRevision) const override;

  virtual bool hasBehavior() const override;
  virtual Glib::ustring getCurrentBehavior() const override;
  virtual void undoableStepBehavior(UNDO::Scope::tTransactionPtr transaction, int direction) override;

  virtual DFBLayout *createLayout(FocusAndMode focusAndMode) const override;
  virtual size_t getHash() const override;

 private:
  const ScaleConverter *createScaleConverter() const;
  uint16_t mapParameterIdToLPCSetting() const;
  void sendModeToLpc() const;
  tControlPositionValue getDefValueAccordingToMode() const;
  void setRoutersModeAccordingToReturnMode();

  PedalModes m_mode = STAY;
  tUpdateID m_updateIdWhenModeChanged = 0;
};
