#pragma once

#include "proxies/hwui/controls/Label.h"
#include <nltools/Types.h>

class Application;
class Parameter;

class ModuleCaption : public Label
{
 private:
  typedef Label super;

 public:
  ModuleCaption(const Rect &pos);

  bool redraw(FrameBuffer &fb) override;

 private:
  virtual void setFontColor(FrameBuffer &fb) const override;
  virtual std::shared_ptr<Font> getFont() const override;
  virtual int getFontHeight() const override;

 protected:
  virtual void updateText(Parameter *newOne);

  StringAndSuffix shortenStringIfNeccessary(std::shared_ptr<Font> font, const StringAndSuffix &text) const override;

 private:
  void onParameterSelected(Parameter *newOne, SignalOrigin signalType);
  void onSelectionChanged();
  void onSoundTypeChanged();
};
