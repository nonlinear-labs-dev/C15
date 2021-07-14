#pragma once

#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationBoundSlider.h>

class LowerModulationBoundSlider : public ModulationBoundSlider
{
  typedef ModulationBoundSlider super;

 public:
  explicit LowerModulationBoundSlider(const Rect &pos);
  ~LowerModulationBoundSlider() override;

 protected:
  BoundPosition getBoundPosition() const override;
  void drawBackground(FrameBuffer &fb) override;
};
