#pragma once

#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationBoundSlider.h>

class UpperModulationBoundSlider : public ModulationBoundSlider
{
  typedef ModulationBoundSlider super;

 public:
  explicit UpperModulationBoundSlider(const Rect &pos);
  ~UpperModulationBoundSlider() override;

 protected:
  BoundPosition getBoundPosition() const override;
};
