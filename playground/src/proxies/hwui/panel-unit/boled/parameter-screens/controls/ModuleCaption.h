#pragma once

#include "proxies/hwui/controls/Label.h"

class Application;
class Parameter;

class ModuleCaption : public Label
{
 private:
  typedef Label super;

 public:
  ModuleCaption(const Rect &pos);
  virtual ~ModuleCaption();

  bool redraw(FrameBuffer &fb) override;

 private:
  virtual void setFontColor(FrameBuffer &fb) const override;
  virtual shared_ptr<Font> getFont() const override;
  virtual int getFontHeight() const override;

  void onParameterSelected(Parameter *newOne);
};
