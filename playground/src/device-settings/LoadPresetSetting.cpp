#include <Application.h>
#include <device-settings/LoadModeSetting.h>
#include <device-settings/EnumSetting.h>
#include <device-settings/Settings.h>
#include <presets/PresetManager.h>

LoadModeSetting::LoadModeSetting(Settings &settings)
    : super(settings, LoadMode::Select)
{
}

const std::vector<ustring> &LoadModeSetting::enumToString() const
{
  static auto ret = std::vector<Glib::ustring>{ "loadtopart", "select", "directload" };
  return ret;
}

const std::vector<ustring> &LoadModeSetting::enumToDisplayString() const
{
  static auto ret = std::vector<Glib::ustring>{ "Select Part", "Select only", "Direct Load" };
  return ret;
}

void LoadModeSetting::cycleForSoundType(SoundType type)
{
  if(type == SoundType::Single)
  {
    switch(get())
    {
      case LoadMode::Select:
      case LoadMode::LoadToPart:
        set(LoadMode::DirectLoad);
        break;
      case LoadMode::DirectLoad:
        set(LoadMode::Select);
        break;
    }
  }
  else
  {
    switch(get())
    {
      case LoadMode::LoadToPart:
        set(LoadMode::Select);
        break;
      case LoadMode::Select:
        set(LoadMode::DirectLoad);
        break;
      case LoadMode::DirectLoad:
        set(LoadMode::LoadToPart);
        break;
    }
  }
}

Glib::ustring LoadModeSetting::getDisplayStringForVoiceGroup(VoiceGroup vg) const
{
  return getDisplayStringForVoiceGroup(vg, get());
}

Glib::ustring LoadModeSetting::getDisplayStringForVoiceGroup(VoiceGroup vg, LoadMode setting) const
{
  switch(setting)
  {
    case LoadMode::Select:
    case LoadMode::DirectLoad:
      return enumToDisplayString()[static_cast<int>(setting)];
    case LoadMode::LoadToPart:
      return UNDO::StringTools::buildString("Load to ", toString(vg));
  }

  return "";
}
