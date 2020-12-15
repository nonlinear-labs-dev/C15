#pragma once
#include <nltools/Types.h>
#include <GroupId.h>
#include "ParameterUseCases.h"
#include "ModParameterUseCases.h"
#include <memory>
#include <playground.h>

class EditBuffer;
class Preset;
class PresetManager;

class EditBufferUseCases
{
 public:
  explicit EditBufferUseCases(EditBuffer* eb);

  void setParameter(const ParameterId& id, tControlPositionValue cp);
  void setSplits(const ParameterId& id, tControlPositionValue cp, tControlPositionValue otherCp);

  void setModulationSource(MacroControls src, const ParameterId& id);
  void setModulationAmount(tControlPositionValue amount, const ParameterId& id);
  void setModAmountAndValue(const ParameterId& id, tControlPositionValue modAmt, tControlPositionValue val);
  void setModulationLimit(const ParameterId& id, tControlPositionValue modAmt, tControlPositionValue paramVal);

  void selectParameter(const ParameterId& id);
  void selectParameter(const Parameter* param);

  void renameMC(const ParameterId& id, const Glib::ustring& name);
  void setMCInfo(const ParameterId& id, const Glib::ustring& info);
  void resetModulation(const ParameterId& id);

  void mutePart(VoiceGroup part);
  void unmutePart(VoiceGroup part);
  void mutePartUnmuteOtherPart(VoiceGroup part);

  void undoableLoad(const Preset* preset);
  void undoableLoadToPart(const Preset* preset, VoiceGroup from, VoiceGroup to);
  void loadSelectedPresetPartIntoPart(VoiceGroup from, VoiceGroup to);

  void resetScaleGroup();
  void resetCustomScale();

  void unlockAllGroups();
  void lockAllGroups();
  void toggleLock(const std::string& groupName);

  std::unique_ptr<ParameterUseCases> getUseCase(ParameterId id);
  std::unique_ptr<ModParameterUseCases> getModParamUseCase(ParameterId id);

 private:
  VoiceGroup invert(VoiceGroup vg);
  PresetManager* getPresetManager() const;

  EditBuffer* m_editBuffer;
};