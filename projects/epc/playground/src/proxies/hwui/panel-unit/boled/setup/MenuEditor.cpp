#include <Application.h>
#include <device-settings/Settings.h>
#include <device-settings/VelocityCurve.h>
#include <proxies/hwui/panel-unit/boled/setup/MenuEditorEntry.h>
#include <proxies/hwui/panel-unit/boled/setup/MenuEditor.h>

MenuEditor::MenuEditor()
    : super(Rect(0, 0, 0, 0))
{
  for(int i = 0; i < 4; i++)
  {
    addControl(new MenuEditorEntry());
  }
}

MenuEditor::~MenuEditor()
{
}

void MenuEditor::setPosition(const Rect &)
{
  static const Rect menuEditorPosition(129, 16, 126, 48);
  super::setPosition(menuEditorPosition);

  int y = 0;
  int h = 12;
  int w = menuEditorPosition.getWidth();
  int x = (menuEditorPosition.getWidth() - w) / 2;

  for(auto &c : getControls())
  {
    c->setPosition(Rect(x, y, w, h));
    y += h;
  }
}

void MenuEditor::onSettingChanged(const Setting *s)
{
  updateOnSettingChanged();
}

void MenuEditor::updateOnSettingChanged()
{
  auto &entries = getDisplayStrings();
  int selectedIndex = getSelectedIndex();
  int assignIndex = selectedIndex - 1;

  for(auto &c : getControls())
  {
    if(auto e = std::dynamic_pointer_cast<MenuEditorEntry>(c))
    {
      e->assign(entries, assignIndex, assignIndex == selectedIndex);
      assignIndex++;
    }
  }
}

bool MenuEditor::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  return false;
}

bool MenuEditor::onRotary(int inc, ButtonModifiers modifiers)
{
  incSetting(inc);
  return true;
}
