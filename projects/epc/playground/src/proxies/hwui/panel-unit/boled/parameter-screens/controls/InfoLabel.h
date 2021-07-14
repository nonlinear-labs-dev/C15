#pragma once

#include <proxies/hwui/controls/Label.h>

class InfoLabel : public Label
{
 private:
  typedef Label super;

 public:
  explicit InfoLabel(const Rect &pos, const Glib::ustring &text = "");

  std::shared_ptr<Font> getFont() const override;
  int getFontHeight() const override;
  void setFontColor(FrameBuffer &fb) const override;
  bool redraw(FrameBuffer &fb) override;
};
