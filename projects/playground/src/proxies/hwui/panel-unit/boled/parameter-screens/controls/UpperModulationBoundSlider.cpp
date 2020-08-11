#include <Application.h>
#include <parameters/ModulateableParameter.h>
#include <presets/EditBuffer.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/UpperModulationBoundSlider.h>
#include <proxies/hwui/HWUI.h>

UpperModulationBoundSlider::UpperModulationBoundSlider(const Rect &pos)
    : super(pos)
{
}

UpperModulationBoundSlider::~UpperModulationBoundSlider()
{
}

UpperModulationBoundSlider::BoundPosition UpperModulationBoundSlider::getBoundPosition() const
{
  if(auto p = dynamic_cast<ModulateableParameter *>(
         Application::get().getPresetManager()->getEditBuffer()->getSelected(getHWUI()->getCurrentVoiceGroup())))
    return p->getModulationAmount() >= 0 ? BoundPosition::Right : BoundPosition::Left;

  return BoundPosition::Right;
}
