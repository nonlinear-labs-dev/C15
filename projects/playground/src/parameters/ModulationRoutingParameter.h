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
  ModulationRoutingParameter(ParameterGroup *group, ParameterId id, tSrcParameterPtr srcParam, tMCParameterPtr tgtParam,
                             const ScaleConverter *scaling);
  ~ModulationRoutingParameter() override;

  void onValueChanged(Initiator initiator, tControlPositionValue oldValue, tControlPositionValue newValue) override;

  void applyLpcPhysicalControl(tControlPositionValue diff);
  void applyAbsoluteLpcPhysicalControl(tControlPositionValue v);

  void undoableRandomize(UNDO::Transaction *transaction, Initiator initiator, double amount) override;

  tMCParameterPtr getTargetParameter() const;
  tSrcParameterPtr getSourceParameter() const;

  bool routes(const PhysicalControlParameter *p) const;
  Glib::ustring getDisplayString() const override;
  tControlPositionValue getControlPositionValue() const override;
  Layout *createLayout(FocusAndMode focusAndMode) const override;

  void onExclusiveRoutingLost();

 private:
  void sendParameterMessage() const override;
};
