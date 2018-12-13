#pragma once

#include "PresetLabel.h"

class PresetNumberLabel : public PresetLabel
{
 private:
  typedef PresetLabel super;

 public:
  PresetNumberLabel(const Rect &pos);
  ~PresetNumberLabel() override;

  void update(int presetNumber, bool selected, bool loaded);

 protected:
  Font::Justification getJustification() const override;
  void drawBackground(FrameBuffer &fb) override;
  int getXOffset() const override;

 private:
  shared_ptr<Font> getFont() const override;
};
