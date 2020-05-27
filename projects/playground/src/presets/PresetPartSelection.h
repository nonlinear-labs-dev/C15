#pragma once
#include <nltools/Types.h>
#include <sigc++/connection.h>

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

  void resetToLoaded();

  const Bank* m_bank { nullptr };
  const Preset* m_preset { nullptr };
  VoiceGroup m_voiceGroup { VoiceGroup::I };
  const VoiceGroup m_focusedVoiceGroup;

 private:
  void onPresetLoaded();
  sigc::connection m_presetLoadedConnection;
};
