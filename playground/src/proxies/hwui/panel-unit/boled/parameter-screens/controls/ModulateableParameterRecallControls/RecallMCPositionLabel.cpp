#include "RecallMCPositionLabel.h"
#include <parameters/ModulateableParameter.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/PresetParameter.h>
#include <presets/EditBuffer.h>
#include <groups/MacroControlsGroup.h>

RecallMCPositionLabel::RecallMCPositionLabel(Rect pos)
    : Label(pos)
{
  if(auto modP
     = dynamic_cast<ModulateableParameter*>(Application::get().getPresetManager()->getEditBuffer()->getSelected()))
  {
    uint16_t id = MacroControlsGroup::modSrcToParamID(modP->getModulationSource());
    if(auto mc = Application::get().getPresetManager()->getEditBuffer()->findParameterByID(id))
    {
      if(auto originalMC = mc->getOriginalParameter())
      {
        auto scPtr = mc->getValue().getScaleConverter();
        auto recallString
            = scPtr->getDimension().stringize(scPtr->controlPositionToDisplay(originalMC->getRecallValue()));
        setText(recallString);
        return;
      }
    }
  }
  setText("");
}
