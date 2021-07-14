#pragma once

#include <proxies/hwui/controls/Label.h>

class PresetNameHeadlineLabel : public Label
{
 private:
  typedef Label super;

 public:
  explicit PresetNameHeadlineLabel(const Rect &pos);

  bool redraw(FrameBuffer &fb) override;
  void setFontColor(FrameBuffer &fb) const override;
  std::shared_ptr<Font> getFont() const override;
  int getFontHeight() const override;
};
