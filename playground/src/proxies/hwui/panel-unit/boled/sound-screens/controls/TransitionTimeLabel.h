#pragma once

#include "proxies/hwui/controls/Label.h"

class Setting;

class TransitionTimeLabel : public Label
{
 private:
  typedef Label super;

 public:
  TransitionTimeLabel(const Rect &rect);
  virtual ~TransitionTimeLabel();

  void setSuffixFontColor(FrameBuffer &fb) const override;

 private:
  void onSettingChanged(const Setting *s);
  void onModifiersChanged();
};
