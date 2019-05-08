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
  virtual ~Caption();

  bool redraw(FrameBuffer &fb) override;

 private:
  virtual void setFontColor(FrameBuffer &fb) const override;
  virtual shared_ptr<Font> getFont() const override;
  virtual int getFontHeight() const override;

  void onParameterSelected(Parameter *newOne);
};
