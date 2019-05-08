#pragma once

#include "proxies/hwui/controls/BarSlider.h"

class Application;

class SelectedParameterBarSlider : public BarSlider
{
 private:
  typedef BarSlider super;

 public:
  // public methods
  SelectedParameterBarSlider(const Rect& rect);
  virtual ~SelectedParameterBarSlider();

 private:
  // private methods
  SelectedParameterBarSlider(const SelectedParameterBarSlider& other);
  SelectedParameterBarSlider& operator=(const SelectedParameterBarSlider&);
};
