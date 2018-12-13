#pragma once

#include "Parameter.h"

class ModulateableParameter : public Parameter
{
  typedef Parameter super;

 public:
  ModulateableParameter(ParameterGroup *group, uint16_t id, const ScaleConverter *scaling, tDisplayValue def,
                        tControlPositionValue coarseDenominator, tControlPositionValue fineDenominator);
  virtual ~ModulateableParameter();

  void writeToLPC(MessageComposer &cmp) const override;
  virtual size_t getHash() const override;

  tDisplayValue getModulationAmount() const;
  void setModulationAmount(UNDO::Transaction *transaction, const tDisplayValue &amount);

  ModulationSource getModulationSource() const;
  void setModulationSource(UNDO::Transaction *transaction, ModulationSource src);

  void undoableSetMCAmountToDefault();

  void undoableSelectModSource(UNDO::Transaction *transaction, ModulationSource src);
  void undoableSetModAmount(UNDO::Transaction *transaction, double amount);

  void undoableIncrementMCSelect(int inc);
  void undoableIncrementMCAmount(int inc);

  void undoableIncrementMCSelect(UNDO::Transaction *transaction, int inc);
  virtual void undoableIncrementMCAmount(UNDO::Transaction *transaction, int inc, ButtonModifiers modifiers);

  void undoableLoadPackedModulationInfo(UNDO::Transaction *transaction, const Glib::ustring &packedModulationInfo);

  void loadDefault(UNDO::Transaction *transaction) override;

  uint16_t getModulationSourceAndAmountPacked() const;
  void applyLpcMacroControl(tDisplayValue diff);

  void copyFrom(UNDO::Transaction *transaction, const PresetParameter *other) override;
  void copyTo(UNDO::Transaction *transaction, PresetParameter *other) const override;

  virtual void exportReaktorParameter(stringstream &target) const override;

  virtual Glib::ustring stringizeModulationAmount() const;
  virtual double getModulationAmountFineDenominator() const;
  virtual double getModulationAmountCoarseDenominator() const;

  std::pair<Glib::ustring, Glib::ustring> getModRangeAsDisplayValues() const;

  static void registerTests();

  virtual DFBLayout *createLayout(FocusAndMode focusAndMode) const override;
  void *getAmountCookie();

  std::pair<tControlPositionValue, tControlPositionValue> getModulationRange(bool clipped) const;

 protected:
  void writeDocProperties(Writer &writer, tUpdateID knownRevision) const override;
  void writeDifferences(Writer &writer, Parameter *other) const override;

 private:
  Glib::ustring modulationValueToDisplayString(tControlPositionValue v) const;
  int getModAmountDenominator(const ButtonModifiers &modifiers) const;

  tDisplayValue m_modulationAmount;
  ModulationSource m_modSource;
};
