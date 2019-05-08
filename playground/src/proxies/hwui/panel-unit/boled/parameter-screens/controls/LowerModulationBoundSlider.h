#pragma once

#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationBoundSlider.h>

class LowerModulationBoundSlider : public ModulationBoundSlider
{
  typedef ModulationBoundSlider super;

 public:
  LowerModulationBoundSlider(const Rect &pos);
  virtual ~LowerModulationBoundSlider();

 protected:
  BoundPosition getBoundPosition() const override;
  void drawBackground(FrameBuffer &fb) override;
};
