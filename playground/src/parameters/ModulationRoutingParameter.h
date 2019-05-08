#pragma once

#include "Parameter.h"

class MacroControlParameter;
class PhysicalControlParameter;

class ModulationRoutingParameter : public Parameter, public IntrusiveListItem<ModulationRoutingParameter *>
{
  typedef Parameter super;

  typedef PhysicalControlParameter *tSrcParameterPtr;
  typedef MacroControlParameter *tMCParameterPtr;

  tMCParameterPtr m_tgtParameter;
  tSrcParameterPtr m_srcParameter;

 public:
  ModulationRoutingParameter(ParameterGroup *group, uint16_t id, tSrcParameterPtr srcParam, tMCParameterPtr tgtParam,
                             const ScaleConverter *scaling);
  virtual ~ModulationRoutingParameter();

  virtual void onValueChanged(Initiator initiator, tControlPositionValue oldValue,
                              tControlPositionValue newValue) override;

  void applyLpcPhysicalControl(tControlPositionValue diff);
  void applyAbsoluteLpcPhysicalControl(tControlPositionValue v);

  virtual void undoableRandomize(UNDO::Scope::tTransactionPtr transaction, Initiator initiator, double amount) override;

  tMCParameterPtr getTargetParameter() const;
  tSrcParameterPtr getSourceParameter() const;

  bool routes(const PhysicalControlParameter *p) const;
  virtual Glib::ustring getDisplayString() const override;
  virtual tControlPositionValue getControlPositionValue() const override;
  virtual DFBLayout *createLayout(FocusAndMode focusAndMode) const override;

  void onExclusiveRoutingLost();
};
