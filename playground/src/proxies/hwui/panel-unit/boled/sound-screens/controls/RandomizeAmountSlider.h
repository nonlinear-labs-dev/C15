#pragma once

#include <proxies/hwui/controls/BarSlider.h>

class Setting;
class Application;

class RandomizeAmountSlider : public BarSlider
{
 private:
  typedef BarSlider super;

 public:
  RandomizeAmountSlider(const Rect &rect);
  virtual ~RandomizeAmountSlider();

 private:
  void onSettingChanged(const Setting *s);
};
