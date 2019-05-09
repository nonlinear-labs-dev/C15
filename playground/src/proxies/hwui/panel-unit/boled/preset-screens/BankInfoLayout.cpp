#include <Application.h>
#include <proxies/hwui/UsageMode.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/controls/ControlOwner.h>
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/BOLED.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/InfoLabel.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/BankInfoCaption.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/BankInfoContent.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/BankNameHeadlineLabel.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/EditBankInfoLayout.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/BankInfoLayout.h>
#include <proxies/hwui/panel-unit/EditPanel.h>
#include <proxies/hwui/panel-unit/PanelUnit.h>

BankInfoLayout::BankInfoLayout()
    : super()
{
}

BankInfoLayout::~BankInfoLayout()
{
}

void BankInfoLayout::addModuleCaption()
{
  addControl(new BankInfoCaption(Rect(0, 0, 64, 13)));
}

void BankInfoLayout::addHeadline()
{
  addControl(new BankNameHeadlineLabel(Rect(64, 0, 128, 13)));
}

void BankInfoLayout::addInfoLabel()
{
  addControl(new InfoLabel(Rect(192, 0, 64, 13)));
}

Scrollable* BankInfoLayout::createScrollableContent()
{
  return new BankInfoContent();
}

bool BankInfoLayout::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  if(i == Buttons::BUTTON_EDIT && down)
  {
    auto layout = new EditBankInfoLayout();
    Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().setOverlay(layout);
    return true;
  }
  else if(i == Buttons::BUTTON_PRESET && down)
  {
    Application::get().getHWUI()->undoableSetFocusAndMode(FocusAndMode(UIFocus::Presets, UIMode::Select));
  }

  return super::onButton(i, down, modifiers);
}
