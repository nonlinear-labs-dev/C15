#pragma once

#include "Parameter.h"

class MacroControlParameter;

class ModulateableParameter : public Parameter
{
  typedef Parameter super;

 public:
  ModulateableParameter(ParameterGroup *group, ParameterId id, const ScaleConverter *scaling, tDisplayValue def,
                        tControlPositionValue coarseDenominator, tControlPositionValue fineDenominator);
  ~ModulateableParameter() override;

  void writeToPlaycontroller(MessageComposer &cmp) const override;
  size_t getHash() const override;

  tDisplayValue getModulationAmount() const;
  virtual void setModulationAmount(UNDO::Transaction *transaction, const tDisplayValue &amount);

  MacroControls getModulationSource() const;
  virtual void setModulationSource(UNDO::Transaction *transaction, MacroControls src);

  virtual void undoableSetMCAmountToDefault();

  virtual void undoableSelectModSource(UNDO::Transaction *transaction, MacroControls src);
  virtual void undoableSetModAmount(UNDO::Transaction *transaction, double amount);

  void loadDefault(UNDO::Transaction *transaction, Defaults mode) override;

  uint16_t getModulationSourceAndAmountPacked() const;
  void applyPlaycontrollerMacroControl(tDisplayValue diff);

  void copyFrom(UNDO::Transaction *transaction, const Parameter *other) override;

  void copyFrom(UNDO::Transaction *transaction, const PresetParameter *other) override;
  void copyTo(UNDO::Transaction *transaction, PresetParameter *other) const override;

  void exportReaktorParameter(std::stringstream &target) const override;

  virtual Glib::ustring stringizeModulationAmount() const;
  virtual Glib::ustring stringizeModulationAmount(tControlPositionValue amt) const;

  virtual double getModulationAmountFineDenominator() const;
  virtual double getModulationAmountCoarseDenominator() const;
  virtual std::pair<Glib::ustring, Glib::ustring> getModRangeAsDisplayValues() const;

  Layout *createLayout(FocusAndMode focusAndMode) const override;
  void *getAmountCookie();

  std::pair<tControlPositionValue, tControlPositionValue> getModulationRange(bool clipped) const;

  bool isChangedFromLoaded() const override;
  bool isAnyModChanged() const;
  bool isModSourceChanged() const;
  bool isModAmountChanged() const;
  bool isMacroControlAssignedAndChanged() const;
  MacroControlParameter *getMacroControl() const;

  bool isDefaultLoaded() const override;

 protected:
  void writeDocProperties(Writer &writer, tUpdateID knownRevision) const override;

  virtual Glib::ustring modulationValueToDisplayString(tControlPositionValue v) const;

 private:
  int getModAmountDenominator(const ButtonModifiers &modifiers) const;

  void sendParameterMessage() const override;

  tDisplayValue m_modulationAmount;
  MacroControls m_modSource;
};
