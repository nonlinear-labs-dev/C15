#pragma once
#include <nltools/Types.h>
#include <presets/PresetPartSelection.h>
#include "PresetListBase.h"
#include <sigc++/connection.h>

class PresetManager;

class LoadToPartPresetList : public PresetListBase
{
  using super = PresetListBase;

 public:
  LoadToPartPresetList(const Rect& pos, bool showBankArrows, PresetPartSelection* partSelectionI,
                       PresetPartSelection* partSelectionII);
  ~LoadToPartPresetList() override;
  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  std::pair<size_t, size_t> getSelectedPosition() const override;
  void onRotary(int inc, ButtonModifiers modifiers) override;

 protected:
  void onVoiceGroupChanged();

  std::array<PresetPartSelection*, 2> m_selections;
  void selectNext();
  void selectPrevious();
  void selectPreviousBank(PresetManager* pm);
  void selectNextBank(PresetManager* pm);

  void onSelectionChanged(const PresetPartSelection& selection);

  void update(const PresetManager* pm);
  void onEnterButtonPressed();

  sigc::connection m_voiceGroupConnection;
  PresetPartSelection* getCurrentSelection();
  const PresetPartSelection* getCurrentSelection() const;
};