#pragma once

#include "Parameter.h"

class ModulationRoutingParameter;

class PhysicalControlParameter : public Parameter
{
  typedef Parameter super;

 public:
  PhysicalControlParameter(ParameterGroup *group, ParameterId id, const ScaleConverter *scaling, tDisplayValue def,
                           int coarseDenominator, int fineDenominator);

  bool isChangedFromLoaded() const override;
  virtual void onChangeFromLpc(tControlPositionValue newValue);
  void registerTarget(ModulationRoutingParameter *target);
  Glib::ustring generateName() const;
  virtual void loadFromPreset(UNDO::Transaction *transaction, const tControlPositionValue &value) override;

  virtual void setCPFromHwui(UNDO::Transaction *transaction, const tControlPositionValue &cpValue) override;
  virtual void setCPFromWebUI(UNDO::Transaction *transaction, const tControlPositionValue &cpValue) override;

  virtual Glib::ustring getDisplayString() const override;

  virtual ReturnMode getReturnMode() const = 0;
  virtual Layout *createLayout(FocusAndMode focusAndMode) const override;
  bool lockingEnabled() const override;
  void setUiSelectedModulationRouter(const ParameterId& paramNumber);
  void toggleUiSelectedModulationRouter(int inc);
  ParameterId getUiSelectedModulationRouter() const;

  virtual void onSelected() override;
  virtual void onUnselected() override;

  virtual bool hasBehavior() const;
  virtual Glib::ustring getCurrentBehavior() const;
  virtual void undoableStepBehavior(UNDO::Transaction *transaction, int direction);

  virtual void undoableRandomize(UNDO::Transaction *transaction, Initiator initiator, double amount) override;

  virtual size_t getHash() const override;
  bool isLocked() const override;

 protected:
  void onValueChanged(Initiator initiator, tControlPositionValue oldValue, tControlPositionValue newValue) override;

 private:
  void sendParameterMessage() const override;

 private:
  IntrusiveList<ModulationRoutingParameter *> m_targets;

  bool m_changingFromHWUI = false;
  bool m_lastChangedFromHWUI = false;
};
