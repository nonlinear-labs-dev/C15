#pragma once

#include "proxies/hwui/controls/BarSlider.h"

class Application;

class SelectedParameterBarSlider : public BarSlider
{
 private:
  typedef BarSlider super;

 public:
  explicit SelectedParameterBarSlider(const Rect& rect);
  ~SelectedParameterBarSlider() override;

 private:
  SelectedParameterBarSlider(const SelectedParameterBarSlider& other);
  SelectedParameterBarSlider& operator=(const SelectedParameterBarSlider&);
};
