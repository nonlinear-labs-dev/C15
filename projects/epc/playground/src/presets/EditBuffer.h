#pragma once

#include "ParameterGroupSet.h"
#include "presets/recall/RecallParameterGroups.h"
#include "nltools/GenericScopeGuard.h"
#include <nltools/threading/Expiration.h>
#include <tools/DelayedJob.h>
#include <tools/Uuid.h>
#include <utility>

class Application;
class Writer;
class PresetManager;
class HWUI;
class SplitPointParameter;
class LoadedPresetLog;

class EditBuffer : public ParameterGroupSet
{
 private:
  typedef ParameterGroupSet super;

 public:
  explicit EditBuffer(PresetManager *parent);
  ~EditBuffer() override;

  Glib::ustring getName() const;
  Glib::ustring getNameWithSuffix() const;
  Glib::ustring getVoiceGroupName(VoiceGroup vg) const;
  Glib::ustring getVoiceGroupNameWithSuffix(VoiceGroup vg, bool addSpace) const;
  size_t getHash() const;
  const Preset *getOrigin() const;
  Parameter *getSelected(VoiceGroup voiceGroup) const;
  int getSelectedParameterNumber() const;
  std::string getPresetOriginDescription() const;

  LoadedPresetLog *getLoadedPresetLog() const;

  bool isZombie() const;

  void setMacroControlValueFromMCView(ParameterId id, double value, const Glib::ustring &uuid);

  void undoableSelectParameter(UNDO::Transaction *transaction, Parameter *p, bool sendReselectionSignal);
  void undoableSelectParameter(UNDO::Transaction *transaction, const ParameterId &id, bool sendReselectionSignal);

  void undoableLoad(UNDO::Transaction *transaction, const Preset *preset, bool sendToAudioEngine);
  void undoableLoadToPart(UNDO::Transaction *trans, const Preset *p, VoiceGroup from, VoiceGroup to);

  void undoableLoadSelectedPreset(UNDO::Transaction *transaction, VoiceGroup loadInto);
  void undoableLoadSelectedToPart(VoiceGroup from, VoiceGroup to);

  void fakeParameterSelectionSignal(VoiceGroup oldGroup, VoiceGroup group);
  void undoableSetLoadedPresetInfo(UNDO::Transaction *transaction, const Preset *preset);
  void undoableUpdateLoadedPresetInfo(UNDO::Transaction *transaction);
  void undoableRandomize(UNDO::Transaction *transaction, Initiator initiator);
  void undoableRandomizePart(UNDO::Transaction *transaction, VoiceGroup currentVoiceGroup, Initiator initiator);
  void undoableInitSound(UNDO::Transaction *transaction, Defaults mode);
  void undoableSetDefaultValues(UNDO::Transaction *transaction, Preset *values);
  void undoableLockAllGroups(UNDO::Transaction *transaction);
  void undoableUnlockAllGroups(UNDO::Transaction *transaction);
  void undoableToggleGroupLock(UNDO::Transaction *transaction, const Glib::ustring &groupId);

  void setName(UNDO::Transaction *transaction, const Glib::ustring &name);
  void setVoiceGroupName(UNDO::Transaction *transaction, const Glib::ustring &name, VoiceGroup vg);

  void writeDocument(Writer &writer, tUpdateID knownRevision) const override;
  Uuid getUUIDOfLastLoadedPreset() const;
  PresetManager *getParent();
  const PresetManager *getParent() const;

  void resetModifiedIndicator(UNDO::Transaction *transaction);
  void resetModifiedIndicator(UNDO::Transaction *transaction, size_t hash);

  void copyFrom(UNDO::Transaction *transaction, const Preset *preset) override;

  tUpdateID onChange(uint64_t flags = UpdateDocumentContributor::ChangeFlags::Generic) override;

  bool hasLocks(VoiceGroup vg) const;
  bool findAnyParameterChanged() const;
  void resetOriginIf(const Preset *p);
  bool isDual() const;

  // CALLBACKS
  sigc::connection onSelectionChanged(const sigc::slot<void, Parameter *, Parameter *> &s,
                                      std::optional<VoiceGroup> initialVG);
  sigc::connection onParameterReselected(const sigc::slot<void, Parameter *> &s);
  sigc::connection onModificationStateChanged(const sigc::slot<void, bool> &s);
  sigc::connection onChange(const sigc::slot<void> &s, bool init = true);
  sigc::connection onPresetLoaded(const sigc::slot<void> &s);
  sigc::connection onLocksChanged(const sigc::slot<void> &s);
  sigc::connection onRecallValuesChanged(const sigc::slot<void> &s);
  sigc::connection onSoundTypeChanged(const sigc::slot<void, SoundType> &s);
  sigc::connection onSoundTypeChanged(const sigc::slot<void, SoundType> &s, bool init);
  sigc::connection onEditBufferConverted(const sigc::slot<void, SoundType> &s);

  bool isModified() const;
  void sendToAudioEngine();

  //RECALL
  RecallParameterGroups &getRecallParameterSet();
  void initRecallValues(UNDO::Transaction *t);

  SoundType getType() const;

  void undoableConvertToDual(UNDO::Transaction *transaction, SoundType type);
  void undoableConvertToSingle(UNDO::Transaction *transaction, VoiceGroup copyFrom);

  void undoableLoadPresetIntoDualSound(UNDO::Transaction *transaction, const Preset *preset, VoiceGroup vg);
  void undoableLoadSinglePresetIntoDualSound(UNDO::Transaction *transaction, const Preset *preset, VoiceGroup to);

  static bool isDualParameterForSoundType(const Parameter *parameter, SoundType type);

  void undoableInitPart(UNDO::Transaction *transaction, VoiceGroup group, Defaults mode);

  void TEST_doDeferredJobs();

  struct PartOrigin
  {
    PartOrigin(Uuid preset, VoiceGroup vg)
        : presetUUID { std::move(preset) }
        , sourceGroup { vg }
    {
    }

    PartOrigin()
        : presetUUID { Uuid::none() }
        , sourceGroup { VoiceGroup::Global }
    {
    }

    Uuid presetUUID;
    VoiceGroup sourceGroup;
  };

  PartOrigin getPartOrigin(VoiceGroup vg) const;

  bool isParameterFocusLocked() const;

 private:
  friend class PresetManager;
  friend class LastLoadedPresetInfoSerializer;

  void undoableLoadPresetPartIntoPart(UNDO::Transaction *transaction, const Preset *preset, VoiceGroup from,
                                      VoiceGroup copyTo);

  Glib::ustring getEditBufferName() const;
  bool findAnyParameterChanged(VoiceGroup vg) const;

  Parameter *searchForAnyParameterWithLock(VoiceGroup vg) const;
  UNDO::Scope &getUndoScope() override;

  void undoableSetType(UNDO::Transaction *transaction, SoundType type);
  void undoableSetTypeFromConvert(UNDO::Transaction *transaction, SoundType type);
  void undoableConvertDualToSingle(UNDO::Transaction *transaction, VoiceGroup copyFrom);
  void undoableConvertLayerToSingle(UNDO::Transaction *transaction, VoiceGroup copyFrom);
  void undoableConvertSplitToSingle(UNDO::Transaction *transaction, VoiceGroup copyFrom);

  void doDeferedJobs();
  void checkModified();

  void lockParameterFocusChanges();
  void unlockParameterFocusChanges();
  void initUnisonVoicesScaling(SoundType newType);

  void initToFX(UNDO::Transaction *transaction);
  void copyAndInitGlobalMasterGroupToPartMasterGroups(UNDO::Transaction *transaction);

  void loadPresetGlobalMasterIntoVoiceGroupMaster(UNDO::Transaction *transaction, const Preset *preset,
                                                  VoiceGroup copyTo);

  void copySumOfMasterGroupToVoiceGroupMasterGroup(UNDO::Transaction *transaction, const Preset *preset,
                                                   VoiceGroup copyFrom, VoiceGroup copyTo);
  void initSplitPoint(UNDO::Transaction *transaction);
  void initFadeFrom(UNDO::Transaction *transaction, VoiceGroup vg);
  void defaultFadeParameters(UNDO::Transaction *transaction);
  void copyVoicesGroups(UNDO::Transaction *transaction, VoiceGroup from, VoiceGroup to);
  void combineSplitPartGlobalMaster(UNDO::Transaction *transaction, VoiceGroup copyFrom);
  void combineLayerPartGlobalMaster(UNDO::Transaction *transaction, VoiceGroup copyFrom);
  void initFadeParameters(UNDO::Transaction *transaction, VoiceGroup group);
  void initCrossFB(UNDO::Transaction *transaction);
  void undoableUnmuteLayers(UNDO::Transaction *transaction);
  void undoableUnisonMonoLoadDefaults(UNDO::Transaction *transaction, VoiceGroup vg);
  void undoableConvertSingleToLayer(UNDO::Transaction *transaction);
  void undoableConvertSingleToSplit(UNDO::Transaction *transaction);
  void undoableConvertLayerToSplit(UNDO::Transaction *transaction);
  void undoableConvertSplitToLayer(UNDO::Transaction *transaction);
  void calculateSplitPointFromFadeParams(UNDO::Transaction *transaction);
  void copySinglePresetMasterToPartMaster(UNDO::Transaction *transaction, const Preset *preset, VoiceGroup targetGroup);
  std::vector<Parameter *> getCrossFBParameters(const VoiceGroup &to) const;
  void loadSinglePresetIntoSplitPart(UNDO::Transaction *transaction, const Preset *preset, VoiceGroup loadInto);
  void loadSinglePresetIntoLayerPart(UNDO::Transaction *transaction, const Preset *preset, VoiceGroup loadTo);
  void undoableLoadPresetPartIntoSplitSound(UNDO::Transaction *transaction, const Preset *preset, VoiceGroup from,
                                            VoiceGroup copyTo);
  void undoableLoadPresetPartIntoLayerSound(UNDO::Transaction *transaction, const Preset *preset, VoiceGroup copyFrom,
                                            VoiceGroup copyTo);
  void undoableLoadPresetPartIntoSingleSound(UNDO::Transaction *transaction, const Preset *preset, VoiceGroup copyFrom,
                                             VoiceGroup copyTo);
  void cleanupParameterSelection(UNDO::Transaction *transaction, SoundType oldType, SoundType newType);
  bool isMonoEnabled(const VoiceGroup &vg) const;
  bool hasMoreThanOneUnisonVoice(const VoiceGroup &vg) const;

  bool isPartLabelChanged(VoiceGroup group) const;
  void cleanupSplitPointIfOldPreset(UNDO::Transaction *transaction, const Preset *p);
  void setSyncSplitSettingAccordingToLoadedPreset(UNDO::Transaction *transaction);

  Signal<void, Parameter *, Parameter *> m_signalSelectedParameter;
  Signal<void, Parameter *> m_signalReselectParameter;
  SignalWithCache<void, bool> m_signalModificationState;
  Signal<void> m_signalChange;
  Signal<void> m_signalPresetLoaded;
  Signal<void> m_signalLocksChanged;
  Signal<void, SoundType> m_signalTypeChanged;
  Signal<void, SoundType> m_signalConversionHappened;

  sigc::connection m_voiceGroupConnection;

  friend class EditBufferSerializer;
  friend class RecallEditBufferSerializer;
  friend class RecallEditBufferSerializer2;
  friend class EditBufferActions;
  friend class PresetParameterGroups;

  friend class EditBufferUseCases;
  friend class SoundUseCases;

  Uuid m_lastLoadedPreset;

  Glib::ustring m_name;
  std::string m_presetOriginDescription;
  std::array<Glib::ustring, 2> m_voiceGroupLabels;

  DelayedJob m_deferredJobs;

  bool m_lockParameterFocusChanges = false;
  bool m_isModified;
  RecallParameterGroups m_recallSet;
  SoundType m_type;
  ParameterId m_lastSelectedParameter;
  size_t m_hashOnStore;

  mutable Preset *m_originCache { nullptr };

  std::unique_ptr<LoadedPresetLog> m_loadedPresetLog;
};
