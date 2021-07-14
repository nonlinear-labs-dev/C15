#include "RecallMCPositionLabel.h"
#include <parameters/ModulateableParameter.h>
#include <Application.h>
#include <proxies/hwui/HWUI.h>
#include <presets/PresetManager.h>
#include <presets/PresetParameter.h>
#include <presets/EditBuffer.h>
#include <groups/MacroControlsGroup.h>
#include <presets/recall/RecallParameter.h>
#include <parameters/scale-converters/ScaleConverter.h>

RecallMCPositionLabel::RecallMCPositionLabel(Rect pos)
    : Label(pos)
{
  if(auto modP = dynamic_cast<ModulateableParameter*>(
         Application::get().getPresetManager()->getEditBuffer()->getSelected(getHWUI()->getCurrentVoiceGroup())))
  {
    auto id = MacroControlsGroup::modSrcToParamId(modP->getModulationSource());
    if(auto mc = Application::get().getPresetManager()->getEditBuffer()->findParameterByID(id))
    {
      if(auto originalMC = mc->getOriginalParameter())
      {
        auto scPtr = mc->getValue().getScaleConverter();
        auto recallString
            = scPtr->getDimension().stringize(scPtr->controlPositionToDisplay(originalMC->getRecallValue()));
        setText(StringAndSuffix { recallString });
        return;
      }
    }
  }
  setText(StringAndSuffix::empty());
}
