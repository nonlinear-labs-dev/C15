#pragma once

#include <proxies/hwui/controls/Label.h>

class Parameter;
class Application;

class HeadlineLabel : public Label
{
 private:
  typedef Label super;

 public:
  HeadlineLabel(const Rect &pos);

  virtual bool redraw(FrameBuffer &fb) override;
  virtual void setFontColor(FrameBuffer &fb) const override;
  virtual std::shared_ptr<Font> getFont() const override;
  virtual int getFontHeight() const override;

 private:
  void updateText(Parameter *oldParam, Parameter *newParam, SignalOrigin signalType);
  void updateMc(const Parameter *param);
};
