#pragma once

#include <proxies/hwui/controls/BarSlider.h>

class Setting;
class Application;

class TransitionTimeSlider : public BarSlider
{
 private:
  typedef BarSlider super;

 public:
  TransitionTimeSlider(const Rect &rect);
  virtual ~TransitionTimeSlider();

 private:
  void onSettingChanged(const Setting *s);
};
