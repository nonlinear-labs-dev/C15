#include <Application.h>
#include <device-settings/Settings.h>
#include <device-settings/ScreenSaverTimeoutSetting.h>
#include "ScreenSaverTimeControls.h"
#include <proxies/hwui/FrameBuffer.h>

//View
ScreenSaverTimeView::ScreenSaverTimeView()
    : SetupLabel({ 0, 0, 0, 0 })
{
  Application::get().getSettings()->getSetting<ScreenSaverTimeoutSetting>()->onChange(
      mem_fun(this, &ScreenSaverTimeView::onSettingChanged));
}

void ScreenSaverTimeView::onSettingChanged(const Setting* s)
{
  setText({ s->getDisplayString(), 0 });
}

//Editor
ScreenSaverTimeEditor::ScreenSaverTimeEditor()
    : super()
{
  getSetting()->onChange(sigc::mem_fun(this, &ScreenSaverTimeEditor::onSettingChanged));
}

ScreenSaverTimeEditor::~ScreenSaverTimeEditor() = default;

void ScreenSaverTimeEditor::incSetting(int inc)
{
  getSetting()->incDec(inc);
}

const std::vector<Glib::ustring>& ScreenSaverTimeEditor::getDisplayStrings() const
{
  return getSetting()->getDisplayStrings();
}

int ScreenSaverTimeEditor::getSelectedIndex() const
{
  return getSetting()->getSelectedIndex();
}

ScreenSaverTimeoutSetting* ScreenSaverTimeEditor::getSetting()
{
  return Application::get().getSettings()->getSetting<ScreenSaverTimeoutSetting>().get();
}
