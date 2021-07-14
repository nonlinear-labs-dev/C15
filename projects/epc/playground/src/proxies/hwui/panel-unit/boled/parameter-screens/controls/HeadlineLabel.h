#pragma once

#include <proxies/hwui/controls/Label.h>

class Parameter;
class Application;

class HeadlineLabel : public Label
{
 private:
  typedef Label super;

 public:
  explicit HeadlineLabel(const Rect &pos);

  bool redraw(FrameBuffer &fb) override;
  void setFontColor(FrameBuffer &fb) const override;
  std::shared_ptr<Font> getFont() const override;
  int getFontHeight() const override;

 private:
  void updateText(Parameter *oldParam, Parameter *newParam);
  void updateMc(const Parameter *param);
};
