#pragma once

#include "proxies/hwui/controls/Label.h"

class Application;
class Parameter;

class Caption : public Label
{
 private:
  typedef Label super;

 public:
  Caption(const Glib::ustring &text, const Rect &pos);
  ~Caption() override;

  bool redraw(FrameBuffer &fb) override;

 private:
  void setFontColor(FrameBuffer &fb) const override;
  std::shared_ptr<Font> getFont() const override;
  int getFontHeight() const override;
};
