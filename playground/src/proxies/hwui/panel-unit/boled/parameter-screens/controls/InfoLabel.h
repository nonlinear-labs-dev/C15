#pragma once

#include <proxies/hwui/controls/Label.h>

class InfoLabel : public Label
{
 private:
  typedef Label super;

 public:
  InfoLabel(const Rect &pos);

  virtual shared_ptr<Font> getFont() const override;
  virtual int getFontHeight() const override;
  virtual void setFontColor(FrameBuffer &fb) const override;
  virtual bool redraw(FrameBuffer &fb) override;
};
