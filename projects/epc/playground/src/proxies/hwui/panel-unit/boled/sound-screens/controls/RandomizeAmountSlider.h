#pragma once

#include <proxies/hwui/controls/BarSlider.h>

class Setting;
class Application;

class RandomizeAmountSlider : public BarSlider
{
 private:
  typedef BarSlider super;

 public:
  explicit RandomizeAmountSlider(const Rect &rect);
  ~RandomizeAmountSlider() override;

 private:
  void onSettingChanged(const Setting *s);
};
