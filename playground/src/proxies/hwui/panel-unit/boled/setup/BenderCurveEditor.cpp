#include <Application.h>
#include <device-settings/BenderCurve.h>
#include <device-settings/Settings.h>
#include <proxies/hwui/panel-unit/boled/setup/BenderCurveEditor.h>
#include <proxies/hwui/panel-unit/boled/setup/MenuEditorEntry.h>

BenderCurveEditor::BenderCurveEditor()
{
  Application::get().getSettings()->getSetting<BenderCurve>()->onChange(
      mem_fun(this, &BenderCurveEditor::onSettingChanged));
}

BenderCurveEditor::~BenderCurveEditor()
{
}

void BenderCurveEditor::incSetting(int inc)
{
  Application::get().getSettings()->getSetting<BenderCurve>()->inc(inc);
}

const vector<ustring> &BenderCurveEditor::getDisplayStrings() const
{
  return Application::get().getSettings()->getSetting<BenderCurve>()->enumToDisplayString();
}

int BenderCurveEditor::getSelectedIndex() const
{
  return (int) Application::get().getSettings()->getSetting<BenderCurve>()->get();
}
