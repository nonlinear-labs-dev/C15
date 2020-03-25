#pragma once

#include "Parameter.h"

class MacroControlParameter;

class ModulateableParameter : public Parameter
{
  typedef Parameter super;

 public:
  ModulateableParameter(ParameterGroup *group, ParameterId id, const ScaleConverter *scaling, tDisplayValue def,
                        tControlPositionValue coarseDenominator, tControlPositionValue fineDenominator);
  virtual ~ModulateableParameter();

  void writeToLPC(MessageComposer &cmp) const override;
  virtual size_t getHash() const override;

  tDisplayValue getModulationAmount() const;
  void setModulationAmount(UNDO::Transaction *transaction, const tDisplayValue &amount);

  MacroControls getModulationSource() const;
  void setModulationSource(UNDO::Transaction *transaction, MacroControls src);

  void undoableSetMCAmountToDefault();

  void undoableSelectModSource(UNDO::Transaction *transaction, MacroControls src);
  void undoableSetModAmount(UNDO::Transaction *transaction, double amount);

  void undoableIncrementMCSelect(int inc);
  virtual void undoableIncrementMCAmount(int inc, ButtonModifiers modifiers);
  void undoableIncrementMCSelect(UNDO::Transaction *transaction, int inc);
  virtual void undoableIncrementMCAmount(UNDO::Transaction *transaction, int inc, ButtonModifiers modifiers);

  void undoableLoadPackedModulationInfo(UNDO::Transaction *transaction, const Glib::ustring &packedModulationInfo);

  void loadDefault(UNDO::Transaction *transaction) override;

  uint16_t getModulationSourceAndAmountPacked() const;
  void applyLpcMacroControl(tDisplayValue diff);

  void copyFrom(UNDO::Transaction *transaction, const Parameter *other) override;

  void copyFrom(UNDO::Transaction *transaction, const PresetParameter *other) override;
  void copyTo(UNDO::Transaction *transaction, PresetParameter *other) const override;

  virtual void exportReaktorParameter(std::stringstream &target) const override;

  virtual Glib::ustring stringizeModulationAmount() const;
  virtual Glib::ustring stringizeModulationAmount(tControlPositionValue amt) const;

  virtual double getModulationAmountFineDenominator() const;
  virtual double getModulationAmountCoarseDenominator() const;
  virtual std::pair<Glib::ustring, Glib::ustring> getModRangeAsDisplayValues() const;

  virtual Layout *createLayout(FocusAndMode focusAndMode) const override;
  void *getAmountCookie();

  std::pair<tControlPositionValue, tControlPositionValue> getModulationRange(bool clipped) const;

  bool isChangedFromLoaded() const override;
  bool isAnyModChanged() const;
  bool isModSourceChanged() const;
  bool isModAmountChanged() const;
  bool isMacroControlAssignedAndChanged() const;
  MacroControlParameter *getMacroControl() const;

  void undoableRecallMCPos();
  void undoableRecallMCSource();
  void undoableRecallMCAmount();

 protected:
  void writeDocProperties(Writer &writer, tUpdateID knownRevision) const override;

  virtual Glib::ustring modulationValueToDisplayString(tControlPositionValue v) const;

 private:
  int getModAmountDenominator(const ButtonModifiers &modifiers) const;

  void sendParameterMessage() const override;

  tDisplayValue m_modulationAmount;
  MacroControls m_modSource;
};
