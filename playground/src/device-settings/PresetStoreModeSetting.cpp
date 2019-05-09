#include "PresetStoreModeSetting.h"

PresetStoreModeSetting::PresetStoreModeSetting(Settings &settings)
    : super(settings, PresetStoreModeSettings::PRESET_STORE_MODE_INSERT)
{
}

PresetStoreModeSetting::~PresetStoreModeSetting()
{
}

const std::vector<Glib::ustring> &PresetStoreModeSetting::enumToString() const
{
  static std::vector<Glib::ustring> s_modeNames = { "append", "overwrite", "insert" };
  return s_modeNames;
}

const std::vector<Glib::ustring> &PresetStoreModeSetting::enumToDisplayString() const
{
  static std::vector<Glib::ustring> s_modeNames = { "Append", "Overwrite", "Insert" };
  return s_modeNames;
}
