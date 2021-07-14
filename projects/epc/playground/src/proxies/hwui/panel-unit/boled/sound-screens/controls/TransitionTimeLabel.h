#pragma once

#include "proxies/hwui/controls/Label.h"

class Setting;

class TransitionTimeLabel : public Label
{
 private:
  typedef Label super;

 public:
  explicit TransitionTimeLabel(const Rect &rect);

  void setSuffixFontColor(FrameBuffer &fb) const override;

 private:
  void onSettingChanged(const Setting *s);
  void onModifiersChanged();
};
