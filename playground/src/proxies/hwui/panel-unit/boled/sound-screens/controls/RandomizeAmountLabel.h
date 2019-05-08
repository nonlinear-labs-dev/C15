#pragma once

#include "proxies/hwui/controls/Label.h"

class Setting;

class RandomizeAmountLabel : public Label
{
 private:
  typedef Label super;

 public:
  RandomizeAmountLabel(const Rect &rect);
  virtual ~RandomizeAmountLabel();

  void setSuffixFontColor(FrameBuffer &fb) const override;

 private:
  void onSettingChanged(const Setting *s);
  void onModifiersChanged();
};
