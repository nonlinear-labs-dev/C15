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
  explicit ModuleCaption(const Rect &pos);
  bool redraw(FrameBuffer &fb) override;

 protected:
  virtual void updateText(Parameter *newOne);
  StringAndSuffix shortenStringIfNeccessary(const std::shared_ptr<Font> &font,
                                            const StringAndSuffix &text) const override;

 private:
  void setFontColor(FrameBuffer &fb) const override;
  std::shared_ptr<Font> getFont() const override;
  int getFontHeight() const override;

  void onParameterSelected(Parameter *newOne);
  void onSelectionChanged();
  void onSoundTypeChanged();
};
