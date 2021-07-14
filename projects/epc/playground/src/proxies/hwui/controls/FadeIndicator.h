#pragma once
#include "Control.h"

class EditBuffer;

class FadeIndicator : public Control
{
 public:
  explicit FadeIndicator(const Rect& r, VoiceGroup vg);
  bool redraw(FrameBuffer& fb) override;
  void drawBackground(FrameBuffer& fb) override;

 protected:
  void onEditBufferChanged(const EditBuffer* eb);

 private:
  const VoiceGroup m_voiceGroup;
  void drawColumn(FrameBuffer& fb, int x, int y) const;
};