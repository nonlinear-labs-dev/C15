#include <Application.h>
#include <parameters/ModulateableParameter.h>
#include <presets/EditBuffer.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/LowerModulationBoundSlider.h>

LowerModulationBoundSlider::LowerModulationBoundSlider(const Rect &pos)
    : super(pos)
{
}

LowerModulationBoundSlider::~LowerModulationBoundSlider()
{
}

void LowerModulationBoundSlider::drawBackground(FrameBuffer &fb)
{
}

LowerModulationBoundSlider::BoundPosition LowerModulationBoundSlider::getBoundPosition() const
{
  if(auto p = dynamic_cast<ModulateableParameter *>(
         Application::get().getPresetManager()->getEditBuffer()->getSelectedParameter()))
    return p->getModulationAmount() < 0 ? BoundPosition::Right : BoundPosition::Left;

  return BoundPosition::Left;
}
