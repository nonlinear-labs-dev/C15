#include <proxies/hwui/controls/Label.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MultiLineLabel.h>
#include "DebugLayout.h"
#include "Application.h"
#include "proxies/hwui/HWUI.h"
#include <glibmm/main.h>

DebugLayout::DebugLayout(Glib::ustring e)
    : Layout(Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled())
{
  auto text = addControl(new MultiLineLabel(e + "\nPress Enter to continue!"));
  auto pos = text->getPosition();
  text->setPosition(Rect(0, 0, 250, 100));
}

bool DebugLayout::onButton(Buttons i, bool down, ::ButtonModifiers modifiers)
{
  if(down && i == Buttons::BUTTON_ENTER)
  {
    Application::get().getMainContext()->wakeup();
    Application::get().getHWUI()->setFocusAndMode(FocusAndMode(UIFocus::Parameters, UIMode::Select, UIDetail::Init));
  }

  return true;
}
