#pragma once
#include <nltools/Types.h>

class Bank;
class Preset;

class PresetPartSelection
{
 public:
  explicit PresetPartSelection(VoiceGroup focus);

  void selectNextBank();
  void selectPreviousBank();

  void selectNextPresetPart();
  void selectPreviousPresetPart();

  const Bank* m_bank { nullptr };
  const Preset* m_preset { nullptr };
  VoiceGroup m_voiceGroup { VoiceGroup::I };
  const VoiceGroup m_focusedVoiceGroup;
};
