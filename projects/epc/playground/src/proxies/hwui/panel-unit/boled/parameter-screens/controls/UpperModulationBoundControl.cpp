#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/UpperModulationBoundControl.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/UpperModulationBoundLabel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/UpperModulationBoundSlider.h>
#include <Application.h>
#include <groups/MacroControlsGroup.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/MacroControlParameter.h>
#include <math.h>
#include <parameters/scale-converters/ScaleConverter.h>
#include <libundo/undo/Scope.h>
#include <proxies/hwui/HWUI.h>

UpperModulationBoundControl::UpperModulationBoundControl(const Rect &r)
    : super(r)
{
  auto height = r.getHeight() / 2;
  addControl(new UpperModulationBoundSlider(Rect(0, 0, r.getWidth(), height)));
  addControl(new UpperModulationBoundLabel(Rect(0, height, r.getWidth(), height)));
}

bool UpperModulationBoundControl::onRotary(int inc, ButtonModifiers modifiers)
{
  auto editBuffer = Application::get().getPresetManager()->getEditBuffer();

  if(auto modulatedParam
     = dynamic_cast<ModulateableParameter *>(editBuffer->getSelected(getHWUI()->getCurrentVoiceGroup())))
  {
    auto mc = modulatedParam->getModulationSource();
    auto mcID = MacroControlsGroup::modSrcToParamId(mc);

    if(auto mcParam = dynamic_cast<MacroControlParameter *>(editBuffer->findParameterByID(mcID)))
    {
      auto range = modulatedParam->getModulationRange(true);

      if(modulatedParam->isBiPolar())
      {
        range.first = 2 * range.first - 1;
        range.second = 2 * range.second - 1;
      }

      auto srcValue = mcParam->getControlPositionValue();
      double denominator = modifiers[FINE] ? modulatedParam->getValue().getFineDenominator()
                                           : modulatedParam->getValue().getCoarseDenominator();

      auto newRight = (round(range.second * denominator) + inc) / denominator;
      newRight = modulatedParam->getValue().getScaleConverter()->getControlPositionRange().clip(newRight);
      auto newModAmount = newRight - range.first;

      auto newValue = range.first + newModAmount * srcValue;

      if(modulatedParam->isBiPolar())
        newModAmount /= 2;

      ModParameterUseCases useCase(modulatedParam);
      useCase.setModulationLimit(newModAmount, newValue);
      return true;
    }
  }
  return false;
}
