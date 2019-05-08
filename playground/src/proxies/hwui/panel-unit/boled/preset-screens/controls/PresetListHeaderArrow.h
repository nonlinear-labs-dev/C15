#pragma once

#include "proxies/hwui/controls/Label.h"

class Application;
class PresetBank;

class PresetListHeaderArrow : public Label
{
 private:
  typedef Label super;

 public:
  PresetListHeaderArrow(const Glib::ustring &text, const Rect &pos);
  virtual ~PresetListHeaderArrow();

  bool redraw(FrameBuffer &fb) override;

 protected:
  void setBackgroundColor(FrameBuffer &fb) const override;
  void setFontColor(FrameBuffer &fb) const override;
  shared_ptr<Font> getFont() const override;
  Font::Justification getJustification() const override;
};
