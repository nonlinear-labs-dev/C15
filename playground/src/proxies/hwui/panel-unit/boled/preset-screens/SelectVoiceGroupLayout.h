#pragma once

#include <proxies/hwui/DFBLayout.h>
#include <nltools/Types.h>

class PresetManagerLayout;
class Button;

class SelectVoiceGroupLayout : public DFBLayout
{
 public:
  SelectVoiceGroupLayout(PresetManagerLayout* parent);

  bool onButton(Buttons i, bool down, ::ButtonModifiers modifiers) override;

 private:
  VoiceGroup m_loadFrom = VoiceGroup::I;

  void toggleSelection();
  void load();
  void cancel();

  Button* m_I;
  Button* m_II;
  PresetManagerLayout* m_parent;
};
