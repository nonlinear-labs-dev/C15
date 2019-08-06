#include <Application.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/controls/ControlOwner.h>
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/BOLED.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/InfoLabel.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetInfoCaption.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetInfoContent.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetNameHeadlineLabel.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/EditPresetInfoLayout.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/PresetInfoLayout.h>
#include <proxies/hwui/panel-unit/EditPanel.h>
#include <proxies/hwui/panel-unit/PanelUnit.h>

PresetInfoLayout::PresetInfoLayout()
    : super()
{
}

PresetInfoLayout::~PresetInfoLayout()
{
}

void PresetInfoLayout::addModuleCaption()
{
  addControl(new PresetInfoCaption(Rect(0, 0, 64, 13)));
}

void PresetInfoLayout::addHeadline()
{
  addControl(new PresetNameHeadlineLabel(Rect(64, 0, 128, 13)));
}

void PresetInfoLayout::addInfoLabel()
{
  addControl(new InfoLabel(Rect(192, 0, 64, 13)));
}

Scrollable* PresetInfoLayout::createScrollableContent()
{
  return new PresetInfoContent();
}

bool PresetInfoLayout::onButton(Buttons button, bool down, ButtonModifiers modifiers)
{
  Layout* l;

  if(down)
  {
    switch(button)
    {
      case Buttons::BUTTON_EDIT:
        l = new EditPresetInfoLayout();
        Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().setOverlay(l);
        return true;
      case Buttons::BUTTON_PRESET:
        Application::get().getHWUI()->undoableSetFocusAndMode(FocusAndMode(UIFocus::Presets, UIMode::Select));
        return true;
    }
  }
  return super::onButton(button, down, modifiers);
}
