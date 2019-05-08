#pragma once

#include "../http/UpdateDocumentContributor.h"
#include "../libundo/undo/Scope.h"
#include "../playground.h"
#include "../proxies/hwui/HWUIEnums.h"
#include "../xml/Writer.h"
#include "PhysicalControlParameter.h"

class RibbonParameter : public PhysicalControlParameter
{
 private:
  typedef PhysicalControlParameter super;

 public:
  using super::super;

  enum RibbonTouchBehaviour
  {
    ABSOLUTE,
    RELATIVE,
    NUM_TOUCH_BEHAVIOURS
  };

  enum RibbonReturnMode
  {
    STAY = 0,
    RETURN = 1,
    NUM_RETURN_MODES
  };

  void undoableSetRibbonTouchBehaviour(UNDO::Scope::tTransactionPtr transaction, RibbonTouchBehaviour mode);
  void undoableSetRibbonTouchBehaviour(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &mode);
  void undoableIncRibbonTouchBehaviour(UNDO::Scope::tTransactionPtr transaction);

  void undoableSetRibbonReturnMode(UNDO::Scope::tTransactionPtr transaction, RibbonReturnMode mode);
  void undoableSetRibbonReturnMode(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &mode);
  void undoableIncRibbonReturnMode(UNDO::Scope::tTransactionPtr transaction);

  RibbonTouchBehaviour getRibbonTouchBehaviour() const;
  RibbonReturnMode getRibbonReturnMode() const;

  virtual ReturnMode getReturnMode() const override;
  virtual void copyFrom(UNDO::Scope::tTransactionPtr transaction, Parameter *other) override;
  virtual void loadDefault(UNDO::Scope::tTransactionPtr transaction) override;

  void boundToMacroControl(tControlPositionValue v);

  virtual DFBLayout *createLayout(FocusAndMode focusAndMode) const override;

 protected:
  void writeDocProperties(Writer &writer, tUpdateID knownRevision) const override;
  void writeDifferences(Writer &writer, Parameter *other) const override;

  virtual void onPresetSentToLpc() const override;
  virtual bool shouldWriteDocProperties(tUpdateID knownRevision) const override;
  virtual bool hasBehavior() const override;
  virtual Glib::ustring getCurrentBehavior() const override;
  virtual void undoableStepBehavior(UNDO::Scope::tTransactionPtr transaction, int direction) override;
  virtual size_t getHash() const override;

 private:
  void ensureExclusiveRoutingIfNeeded();
  void sendModeToLpc() const;
  const ScaleConverter *createScaleConverter() const;
  tControlPositionValue getDefaultValueAccordingToMode() const;
  void setupScalingAndDefaultValue();

  RibbonTouchBehaviour m_touchBehaviour = ABSOLUTE;
  RibbonReturnMode m_returnMode = STAY;
  tUpdateID m_updateIdWhenModeChanged = 0;
};
