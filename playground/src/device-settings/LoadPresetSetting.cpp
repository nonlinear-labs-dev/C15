#include <Application.h>
#include <device-settings/LoadModeSetting.h>
#include <device-settings/EnumSetting.h>
#include <device-settings/Settings.h>
#include <presets/PresetManager.h>

LoadModeSetting::LoadModeSetting(Settings &settings)
    : super(settings, LoadMode::Select)
    , m_dualSoundOrder{ LoadMode::LoadToPart, LoadMode::Select, LoadMode::DirectLoad }
    , m_singleSoundOrder{ LoadMode::Select, LoadMode::DirectLoad }
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
  auto current = static_cast<int>(get());
  current++;

  if(type == SoundType::Single)
    updateLoadMode(current, m_singleSoundOrder);
  else
    updateLoadMode(current, m_dualSoundOrder);
}

void LoadModeSetting::antiCycleForSoundType(SoundType type)
{
  auto current = static_cast<int>(get());
  

  if(type == SoundType::Single)
    updateLoadMode(current, m_singleSoundOrder);
  else
    updateLoadMode(current, m_dualSoundOrder);
}

template <typename T> void LoadModeSetting::updateLoadMode(int current, const T &loadModeOrder)
{
  if(current == loadModeOrder.size())
    current = 0;

  if(current <= 0)
    current = loadModeOrder.size();

  set(loadModeOrder[current]);
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
