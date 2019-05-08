#include <Application.h>
#include <device-settings/AutoLoadSelectedPreset.h>
#include <device-settings/EnumSetting.h>
#include <device-settings/Settings.h>
#include <presets/PresetManager.h>

AutoLoadSelectedPreset::AutoLoadSelectedPreset(Settings &settings)
    : super(settings, false)
{
}

AutoLoadSelectedPreset::~AutoLoadSelectedPreset()
{
}

void AutoLoadSelectedPreset::cheat(tEnum m)
{
  super::set(m);
}

bool AutoLoadSelectedPreset::set(tEnum m)
{
  bool ret = super::set(m);

  if(m == BooleanSettings::BOOLEAN_SETTING_TRUE && !getSettings()->isLoading())
  {
    Application::get().getPresetManager()->doAutoLoadSelectedPreset();
  }

  return ret;
}
