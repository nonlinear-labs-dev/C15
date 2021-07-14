#include "RecallMCAmountLabel.h"
#include <parameters/ModulateableParameter.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/PresetParameter.h>
#include <presets/EditBuffer.h>
#include <groups/MacroControlsGroup.h>
#include <presets/recall/RecallParameter.h>
#include <proxies/hwui/HWUI.h>

RecallMCAmountLabel::RecallMCAmountLabel(Rect pos)
    : Label(pos)
{
  if(auto modP = dynamic_cast<ModulateableParameter*>(
         Application::get().getPresetManager()->getEditBuffer()->getSelected(getHWUI()->getCurrentVoiceGroup())))
  {
    if(auto originalParam = modP->getOriginalParameter())
    {
      auto ogModAmt = originalParam->getRecallModulationAmount();
      setText(StringAndSuffix { modP->stringizeModulationAmount(ogModAmt) });
      return;
    }
  }
  setText(StringAndSuffix::empty());
}
