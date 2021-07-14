#pragma once

#include <proxies/hwui/controls/BarSlider.h>

class Setting;
class Application;

class TransitionTimeSlider : public BarSlider
{
 private:
  typedef BarSlider super;

 public:
  explicit TransitionTimeSlider(const Rect &rect);
  ~TransitionTimeSlider() override;

 private:
  void onSettingChanged(const Setting *s);
};
