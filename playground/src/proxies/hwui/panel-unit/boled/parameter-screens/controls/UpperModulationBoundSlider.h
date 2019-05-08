#pragma once

#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationBoundSlider.h>

class UpperModulationBoundSlider : public ModulationBoundSlider
{
  typedef ModulationBoundSlider super;

 public:
  UpperModulationBoundSlider(const Rect &pos);
  virtual ~UpperModulationBoundSlider();

 protected:
  BoundPosition getBoundPosition() const override;
};
