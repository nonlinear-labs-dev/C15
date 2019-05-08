#pragma once

#include "proxies/hwui/controls/DotSlider.h"

class Application;

class SelectedParameterDotSlider : public DotSlider
{
 private:
  typedef DotSlider super;

 public:
  SelectedParameterDotSlider(const Rect& rect);
  virtual ~SelectedParameterDotSlider();

 private:
  SelectedParameterDotSlider(const SelectedParameterDotSlider& other);
  SelectedParameterDotSlider& operator=(const SelectedParameterDotSlider&);
};
