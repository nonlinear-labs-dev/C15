#pragma once

#include "Parameter.h"
#include <proxies/hwui/HWUIEnums.h>
#include <set>
#include <nltools/threading/Throttler.h>

class ModulateableParameter;

class MacroControlParameter : public Parameter
{
  typedef Parameter super;

 public:
  MacroControlParameter(ParameterGroup *group, const ParameterId &id);
  ~MacroControlParameter() override;

  typedef std::set<ModulateableParameter *> tTargets;

  void writeDocProperties(Writer &writer, tUpdateID knownRevision) const override;
  void writeDifferences(Writer &writer, Parameter *other) const override;

  void registerTarget(ModulateableParameter *target);
  void unregisterTarget(ModulateableParameter *target);

  void setCPFromMCView(UNDO::Transaction *transaction, const tControlPositionValue &cpValue);
  void undoableSetGivenName(UNDO::Transaction *transaction, const Glib::ustring &newName);
  void undoableSetInfo(UNDO::Transaction *transaction, const Glib::ustring &newName);

  void undoableRandomize(UNDO::Transaction *transaction, Initiator initiator, double amount) override;
  void undoableResetConnectionsToTargets();
  const Glib::ustring &getGivenName() const;
  const Glib::ustring &getInfo() const;

  void loadDefault(UNDO::Transaction *transaction, Defaults mode) override;
  void copyFrom(UNDO::Transaction *transaction, const PresetParameter *other) override;
  void copyTo(UNDO::Transaction *transaction, PresetParameter *other) const override;
  Glib::ustring getLongName() const override;

  const tTargets &getTargets() const;
  sigc::connection onTargetListChanged(sigc::slot<void> cb);

  bool isSourceOfTargetIn(const std::list<ParameterId> &ids) const;
  bool isSourceOf(ParameterId id) const;

  void onSelected() override;

  void onUnselected() override;

  static ParameterId getLastSelectedMacroControl();
  static ParameterId getMCSmoothingParameterForMC(const ParameterId &mcId);
  ParameterId getSmoothingParameter();

  void applyAbsolutePlaycontrollerPhysicalControl(tControlPositionValue v);
  void applyPlaycontrollerPhysicalControl(tControlPositionValue diff);

  void onValueChanged(Initiator initiator, tControlPositionValue oldValue, tControlPositionValue newValue) override;
  void setUiSelectedHardwareSource(ParameterId paramNumber);
  void toggleUiSelectedHardwareSource(int inc);
  ParameterId getUiSelectedHardwareSource() const;

  Layout *createLayout(FocusAndMode focusAndMode) const override;

  size_t getHash() const override;

  void setLastMCViewUUID(const Glib::ustring &uuid);

  void updateMCViewsFromMCChange(const Initiator &initiator);
  bool isChangedFromLoaded() const override;

 private:
  friend class MacroControlParameterUseCases;

  void updateBoundRibbon();
  void onValueFineQuantizedChanged(Initiator initiator, tControlPositionValue oldValue,
                                   tControlPositionValue newValue) override;

  tTargets m_targets;
  ParameterId m_UiSelectedHardwareSourceParameterID { 254, VoiceGroup::Global };
  Glib::ustring m_givenName;
  Glib::ustring m_info;
  Glib::ustring m_lastMCViewUuid;

  void propagateMCChangeToMCViews(const Initiator &initiatior);

  void sendParameterMessage() const override;

  tControlPositionValue lastBroadcastedControlPosition = std::numeric_limits<tControlPositionValue>::max();

  Throttler mcviewThrottler;

  sigc::signal<void> m_targetListChanged;
};
