#pragma once

#include "proxies/hwui/controls/DotSlider.h"

class Application;

class SelectedParameterDotSlider : public DotSlider
{
 private:
  typedef DotSlider super;

 public:
  explicit SelectedParameterDotSlider(const Rect& rect);
  ~SelectedParameterDotSlider() override;

  SelectedParameterDotSlider(const SelectedParameterDotSlider& other) = delete;
  SelectedParameterDotSlider& operator=(const SelectedParameterDotSlider&) = delete;
};
