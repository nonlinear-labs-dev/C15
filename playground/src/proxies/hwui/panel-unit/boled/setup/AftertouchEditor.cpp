#include <Application.h>
#include <device-settings/Settings.h>
#include <device-settings/AftertouchCurve.h>
#include <proxies/hwui/panel-unit/boled/setup/MenuEditorEntry.h>
#include <proxies/hwui/panel-unit/boled/setup/AftertouchEditor.h>

AftertouchEditor::AftertouchEditor()
{
  Application::get().getSettings()->getSetting<AftertouchCurve>()->onChange(
      mem_fun(this, &AftertouchEditor::onSettingChanged));
}

AftertouchEditor::~AftertouchEditor()
{
}

void AftertouchEditor::incSetting(int inc)
{
  Application::get().getSettings()->getSetting<AftertouchCurve>()->inc(inc);
}

const vector<ustring> &AftertouchEditor::getDisplayStrings() const
{
  return Application::get().getSettings()->getSetting<AftertouchCurve>()->enumToDisplayString();
}

int AftertouchEditor::getSelectedIndex() const
{
  return (int) Application::get().getSettings()->getSetting<AftertouchCurve>()->get();
}
