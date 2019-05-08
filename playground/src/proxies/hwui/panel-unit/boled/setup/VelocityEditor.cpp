#include <Application.h>
#include <device-settings/Settings.h>
#include <device-settings/VelocityCurve.h>
#include <proxies/hwui/panel-unit/boled/setup/MenuEditorEntry.h>
#include <proxies/hwui/panel-unit/boled/setup/VelocityEditor.h>

VelocityEditor::VelocityEditor()
{
  Application::get().getSettings()->getSetting<VelocityCurve>()->onChange(
      mem_fun(this, &VelocityEditor::onSettingChanged));
}

VelocityEditor::~VelocityEditor()
{
}

void VelocityEditor::incSetting(int inc)
{
  Application::get().getSettings()->getSetting<VelocityCurve>()->inc(inc);
}

const vector<ustring> &VelocityEditor::getDisplayStrings() const
{
  return Application::get().getSettings()->getSetting<VelocityCurve>()->enumToDisplayString();
}

int VelocityEditor::getSelectedIndex() const
{
  return (int) Application::get().getSettings()->getSetting<VelocityCurve>()->get();
}
