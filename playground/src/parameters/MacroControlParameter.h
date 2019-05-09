#pragma once

#include "Parameter.h"
#include <proxies/hwui/HWUIEnums.h>
#include <set>
#include <tools/Throttler.h>

class ModulateableParameter;

class MacroControlParameter : public Parameter
{
  typedef Parameter super;

 public:
  MacroControlParameter(ParameterGroup *group, uint16_t id);
  ~MacroControlParameter() override;

  typedef std::set<ModulateableParameter *> tTargets;

  void writeDocProperties(Writer &writer, tUpdateID knownRevision) const override;
  void writeDifferences(Writer &writer, Parameter *other) const override;

  void registerTarget(ModulateableParameter *target);
  void unregisterTarget(ModulateableParameter *target);

  void setCPFromMCView(UNDO::Transaction *transaction, const tControlPositionValue &cpValue);
  void undoableSetGivenName(UNDO::Transaction *transaction, const ustring &newName);
  void undoableSetInfo(const Glib::ustring &newName);
  void undoableSetInfo(UNDO::Transaction *transaction, const Glib::ustring &newName);

  void undoableRandomize(UNDO::Transaction *transaction, Initiator initiator, double amount) override;
  void undoableResetConnectionsToTargets();
  const Glib::ustring &getGivenName() const;
  const Glib::ustring &getInfo() const;

  void loadDefault(UNDO::Transaction *transaction) override;
  void copyFrom(UNDO::Transaction *transaction, const PresetParameter *other) override;
  void copyTo(UNDO::Transaction *transaction, PresetParameter *other) const override;
  Glib::ustring getLongName() const override;

  const tTargets &getTargets() const;
  sigc::connection onTargetListChanged(sigc::slot<void> cb);

  bool isSourceOfTargetIn(const std::list<gint32> &ids) const;
  bool isSourceOf(gint32 id) const;

  void onSelected() override;

  void onUnselected() override;

  static int getLastSelectedMacroControl();

  DFBLayout *createLayout(FocusAndMode focusAndMode) const override;

  size_t getHash() const override;

  void setLastMCViewUUID(const Glib::ustring &uuid);

  void updateMCViewsFromMCChange(const Initiator &initiator);

 private:
  void updateBoundRibbon();
  void onValueFineQuantizedChanged(Initiator initiator, tControlPositionValue oldValue,
                                   tControlPositionValue newValue) override;

  tTargets m_targets;
  int m_UiSelectedHardwareSourceParameterID;
  Glib::ustring m_givenName;
  Glib::ustring m_info;
  Glib::ustring m_lastMCViewUuid;

  void propagateMCChangeToMCViews(const Initiator &initiatior);
  tControlPositionValue lastBroadcastedControlPosition = std::numeric_limits<tControlPositionValue>::max();

  Throttler mcviewThrottler;

  sigc::signal<void> m_targetListChanged;
};
