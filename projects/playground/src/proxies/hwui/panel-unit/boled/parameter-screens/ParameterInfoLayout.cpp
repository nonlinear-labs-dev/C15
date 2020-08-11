#include <Application.h>
#include <parameters/MacroControlParameter.h>
#include <presets/EditBuffer.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/controls/ControlOwner.h>
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/BOLED.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/HeadlineLabel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/InfoLabel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModuleCaption.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ParameterInfoText.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/EditMCInfoLayout.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterInfoLayout.h>
#include <proxies/hwui/panel-unit/EditPanel.h>
#include <proxies/hwui/panel-unit/PanelUnit.h>

ParameterInfoLayout::ParameterInfoLayout()
    : super()
{
}

void ParameterInfoLayout::addHeadline()
{
  addControl(new HeadlineLabel(Rect(64, 0, 128, 13)));
}

void ParameterInfoLayout::addInfoLabel()
{
  addControl(new InfoLabel(Rect(192, 0, 64, 13)));
}

void ParameterInfoLayout::addModuleCaption()
{
  addControl(new ModuleCaption(Rect(0, 0, 64, 13)));
}

Scrollable *ParameterInfoLayout::createScrollableContent()
{
  return new ParameterInfoText(this);
}

bool ParameterInfoLayout::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  if(i == Buttons::BUTTON_EDIT && down)
  {
    if(dynamic_cast<MacroControlParameter *>(
           Application::get().getPresetManager()->getEditBuffer()->getSelected(getHWUI()->getCurrentVoiceGroup())))
    {
      Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().setOverlay(new EditMCInfoLayout());
      return true;
    }
  }

  return super::onButton(i, down, modifiers);
}
