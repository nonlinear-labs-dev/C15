#pragma once

#include "Parameter.h"

class MacroControlParameter;

class ModulateableParameter : public Parameter
{
  typedef Parameter super;

 public:
  ModulateableParameter(ParameterGroup *group, uint16_t id, const ScaleConverter *scaling, tDisplayValue def,
                        tControlPositionValue coarseDenominator, tControlPositionValue fineDenominator);
  virtual ~ModulateableParameter();

  enum ModulationSource
  {
    NONE = 0,
    MC1 = 1,
    MC2 = 2,
    MC3 = 3,
    MC4 = 4,
    NUM_CHOICES = 5
  };

  void writeToLPC(MessageComposer &cmp) const override;
  virtual size_t getHash() const override;

  tDisplayValue getModulationAmount() const;
  void setModulationAmount(UNDO::Scope::tTransactionPtr transaction, const tDisplayValue &amount);

  ModulationSource getModulationSource() const;
  void setModulationSource(UNDO::Scope::tTransactionPtr transaction, ModulationSource src);

  void undoableSetMCAmountToDefault();

  void undoableSelectModSource(UNDO::Scope::tTransactionPtr transaction, int src);
  void undoableSetModAmount(UNDO::Scope::tTransactionPtr transaction, double amount);

  void undoableIncrementMCSelect(int inc);
  void undoableIncrementMCAmount(int inc);

  void undoableIncrementMCSelect(UNDO::Scope::tTransactionPtr transaction, int inc);
  virtual void undoableIncrementMCAmount(UNDO::Scope::tTransactionPtr transaction, int inc, ButtonModifiers modifiers);

  void undoableLoadPackedModulationInfo(UNDO::Scope::tTransactionPtr transaction,
                                        const Glib::ustring &packedModulationInfo);

  void loadDefault(UNDO::Scope::tTransactionPtr transaction) override;

  uint16_t getModulationSourceAndAmountPacked() const;
  void applyLpcMacroControl(tDisplayValue diff);

  void copyFrom(UNDO::Scope::tTransactionPtr transaction, Parameter *other) override;
  virtual void exportReaktorParameter(stringstream &target) const override;

  virtual Glib::ustring stringizeModulationAmount() const;
  virtual double getModulationAmountFineDenominator() const;
  virtual double getModulationAmountCoarseDenominator() const;
  virtual Glib::ustring getModAmountAsDisplayValue() const;

  std::pair<Glib::ustring, Glib::ustring> getModRangeAsDisplayValues() const;

  static void registerTests();

  virtual DFBLayout *createLayout(FocusAndMode focusAndMode) const override;
  void *getAmountCookie();

  std::pair<tControlPositionValue, tControlPositionValue> getModulationRange() const;

  MacroControlParameter *getMacroControl() const;

 protected:
  void writeDocProperties(Writer &writer, tUpdateID knownRevision) const override;
  void writeDifferences(Writer &writer, Parameter *other) const override;

 private:
  Glib::ustring modulationValueToDisplayString(tControlPositionValue v) const;

  tDisplayValue m_modulationAmount;
  ModulationSource m_modSource;
};
