#pragma once

#include "PresetLabel.h"
#include <nltools/Types.h>
#include <presets/Preset.h>

class SinglePresetTypeLabel : public PresetLabel
{
 public:
  explicit SinglePresetTypeLabel(const Rect& r);

  void update(const Preset* newPreset);

 protected:
  Font::Justification getJustification() const override;

 protected:
  int getXOffset() const override;
};

class DualPresetTypeLabel : public Control
{
 public:
  explicit DualPresetTypeLabel(const Rect& r);
  bool redraw(FrameBuffer& fb) override;
  bool drawLayer(FrameBuffer& buffer);
  bool drawSplit(FrameBuffer& buffer);
  void update(const Preset* selected);

 private:
  const FrameBufferColors selectColor = FrameBufferColors::C255;
  const FrameBufferColors loadedColor = FrameBufferColors::C179;
  const FrameBufferColors defaultColor = FrameBufferColors::C77;

  bool m_anySelected = false;
  bool m_selectedI = false;
  bool m_selectedII = false;
  bool m_presetLoaded = false;
  bool m_inidicateI = false;
  bool m_inidicateII = false;

  bool hasPreset = false;

  SoundType m_presetType = SoundType::Single;
  bool drawSingle(FrameBuffer& buffer);
};

class PresetTypeLabel : public Control
{
 public:
  explicit PresetTypeLabel(const Rect& pos);
  ~PresetTypeLabel() override;

  void update(const Preset* newSelection);
  bool redraw(FrameBuffer& fb) override;
  void drawBackground(FrameBuffer& fb) override;

 protected:
  void onEditBufferChanged();
  void onVoiceGroupChanged(const VoiceGroup& vg);
  sigc::connection m_editbufferConnection;
  sigc::connection m_voiceGroupChanged;
  std::unique_ptr<Control> m_currentControl;
  const Preset* selectedPreset { nullptr };
};
