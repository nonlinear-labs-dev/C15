#include "PresetStoreModeSetting.h"

PresetStoreModeSetting::PresetStoreModeSetting(Settings &settings)
    : super(settings, PresetStoreModeSettings::PRESET_STORE_MODE_INSERT)
{
}

PresetStoreModeSetting::~PresetStoreModeSetting()
{
}

const vector<Glib::ustring> &PresetStoreModeSetting::enumToString() const
{
  static vector<Glib::ustring> s_modeNames = { "append", "overwrite", "insert" };

  return s_modeNames;
}

const vector<Glib::ustring> &PresetStoreModeSetting::enumToDisplayString() const
{
  static vector<Glib::ustring> s_modeNames = { "Append", "Overwrite", "Insert" };

  return s_modeNames;
}
