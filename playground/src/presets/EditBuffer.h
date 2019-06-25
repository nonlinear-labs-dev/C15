#pragma once

#include "ParameterGroupSet.h"
#include "presets/recall/RecallParameterGroups.h"
#include <tools/Expiration.h>
#include <tools/DelayedJob.h>
#include <tools/Uuid.h>

class Application;
class Writer;
class PresetManager;

class EditBuffer : public ParameterGroupSet
{
 private:
  typedef ParameterGroupSet super;

 public:
  EditBuffer(PresetManager *parent);
  ~EditBuffer() override;

  Glib::ustring getName() const;
  size_t getHash() const;
  const Preset *getOrigin() const;
  Parameter *getSelected() const;
  bool isZombie() const;

  void setMacroControlValueFromMCView(int id, double value, Glib::ustring uuid);
  void undoableClear(UNDO::Transaction *transaction);
  void undoableSelectParameter(const Glib::ustring &id);
  void undoableSelectParameter(uint16_t id);
  void undoableSelectParameter(Parameter *p);
  void undoableSelectParameter(UNDO::Transaction *transaction, Parameter *p);
  void undoableLoad(UNDO::Transaction *transaction, Preset *preset);
  void undoableLoad(Preset *preset);
  void undoableLoadSelectedPreset();
  void undoableSetLoadedPresetInfo(UNDO::Transaction *transaction, Preset *preset);
  void undoableUpdateLoadedPresetInfo(UNDO::Transaction *transaction);
  void undoableRandomize(UNDO::Transaction *transaction, Initiator initiator);
  void undoableInitSound(UNDO::Transaction *transaction);
  void undoableSetDefaultValues(UNDO::Transaction *transaction, Preset *values);
  void undoableLockAllGroups(UNDO::Transaction *transaction);
  void undoableUnlockAllGroups(UNDO::Transaction *transaction);
  void undoableToggleGroupLock(UNDO::Transaction *transaction, const Glib::ustring &groupId);

  void setName(UNDO::Transaction *transaction, const Glib::ustring &name);

  void writeDocument(Writer &writer, tUpdateID knownRevision) const override;
  Uuid getUUIDOfLastLoadedPreset() const;
  PresetManager *getParent();
  const PresetManager *getParent() const;

  void resetModifiedIndicator(UNDO::Transaction *transaction);
  void resetModifiedIndicator(UNDO::Transaction *transaction, size_t hash);

  void copyFrom(UNDO::Transaction *transaction, const Preset *preset);

  tUpdateID onChange(uint64_t flags = UpdateDocumentContributor::ChangeFlags::Generic) override;

  bool hasLocks() const;
  bool anyParameterChanged() const;
  void resetOriginIf(const Preset *p);

  // CALLBACKS
  sigc::connection onSelectionChanged(slot<void, Parameter *, Parameter *> s);
  sigc::connection onModificationStateChanged(slot<void, bool> s);
  sigc::connection onChange(slot<void> s);
  sigc::connection onPresetLoaded(slot<void> s);
  sigc::connection onLocksChanged(slot<void> s);
  sigc::connection onRecallValuesChanged(slot<void> s);

  void undoableImportReaktorPreset(const Glib::ustring &preset);
  void undoableImportReaktorPreset(UNDO::Transaction *transaction, const Glib::ustring &preset);
  Glib::ustring exportReaktorPreset();
  bool isModified() const;
  void sendToLPC();

  //RECALL
  RecallParameterGroups &getRecallParameterSet();
  void initRecallValues(UNDO::Transaction *t);

 private:
  Parameter *searchForAnyParameterWithLock() const;

  UNDO::Scope &getUndoScope() override;

  void setParameter(size_t id, double cpValue);

  void undoableSelectParameter(UNDO::Transaction *transaction, const Glib::ustring &id);
  void setModulationSource(ModulationSource src);
  void setModulationAmount(double amount);

  bool undoableImportReaktorParameter(UNDO::Transaction *transaction, std::istringstream &input, Parameter *param);
  bool readReaktorPresetHeader(std::istringstream &input) const;

  void doDeferedJobs();
  void checkModified();

  Signal<void, Parameter *, Parameter *> m_signalSelectedParameter;
  SignalWithCache<void, bool> m_signalModificationState;
  Signal<void> m_signalChange;
  Signal<void> m_signalPresetLoaded;
  Signal<void> m_signalLocksChanged;

  Parameter *m_selectedParameter = nullptr;

  friend class EditBufferSerializer;
  friend class RecallEditBufferSerializer;
  friend class EditBufferActions;
  friend class PresetParameterGroups;

  Uuid m_lastLoadedPreset;
  tUpdateID m_updateIdWhenLastLoadedPresetChanged = 0;

  Glib::ustring m_name;
  DelayedJob m_deferedJobs;

  bool m_isModified;
  size_t m_hashOnStore;

  mutable Preset *m_originCache = nullptr;
  RecallParameterGroups m_recallSet;

  friend class PresetManager;
  friend class LastLoadedPresetInfoSerializer;
};
