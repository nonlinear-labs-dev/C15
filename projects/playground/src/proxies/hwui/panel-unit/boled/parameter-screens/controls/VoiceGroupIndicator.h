#pragma once
#include <proxies/hwui/controls/Control.h>

class VoiceGroupIndicator : public Control
{
 public:
  explicit VoiceGroupIndicator(const Rect& r);
  ~VoiceGroupIndicator() override;
  bool redraw(FrameBuffer& fb) override;

 private:
  void onSoundTypeChanged(SoundType type);
  void onParameterChanged(const Parameter* parameter);
  void onParameterSelectionChanged(const Parameter* old, const Parameter* newParam);
  void onVoiceGroupSelectionChanged(VoiceGroup vg);
  void onLoadModeChanged(bool loadModeActive);

  bool drawSplit(FrameBuffer& fb);
  bool drawLayer(FrameBuffer& fb);

  bool isLayerPartMuted(VoiceGroup vg) const;
  bool shouldDraw();

  VoiceGroup m_selectedVoiceGroup {};
  SoundType m_currentSoundType {};
  bool m_inLoadToPart = false;

  sigc::connection m_parameterChanged;
  const Parameter* m_param = nullptr;
};
