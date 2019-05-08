#include "PresetSettings.h"
#include "PresetSetting.h"
#include "Preset.h"
#include "xml/Writer.h"
#include "groups/HardwareSourcesGroup.h"

PresetSettings::PresetSettings(Preset &preset)
    : super(&preset)
{
}

PresetSettings::~PresetSettings()
{
}

void PresetSettings::add(const Glib::ustring &key, PresetSetting *s)
{
  m_settings[key] = tSetting(s);
}

void PresetSettings::set(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &key, const Glib::ustring &value)
{
  if(auto setting = getSetting(key))
    setting->load(transaction, value);
}

PresetSettings::tSetting PresetSettings::getSetting(const Glib::ustring &key)
{
  auto it = m_settings.find(key);
  if(it != m_settings.end())
    return it->second;

  return nullptr;
}

PresetSettings::tSettings &PresetSettings::getSettings()
{
  return m_settings;
}

const PresetSettings::tSettings &PresetSettings::getSettings() const
{
  return m_settings;
}

void PresetSettings::writeDocument(Writer &writer, tUpdateID knownRevision) const
{
  bool changed = knownRevision < getUpdateIDOfLastChange();

  writer.writeTag("preset-settings", Attribute("changed", changed), [&]() {
    if(changed)
    {
      for(auto &setting : m_settings)
      {
        writer.writeTag(setting.first, [&]() { setting.second->writeDocument(writer, knownRevision); });
      }
    }
  });
}

void PresetSettings::sendToLPC() const
{
  for(auto &s : m_settings)
    s.second->sendToLPC();
}

void PresetSettings::copyFrom(UNDO::Scope::tTransactionPtr transaction, const PresetSettings &other)
{
  for(const auto &it : other.getSettings())
    set(transaction, it.first, it.second->save());
}

size_t PresetSettings::getHash() const
{
  size_t hash = 0;

  for(auto &s : m_settings)
  {
    hash_combine(hash, s.first);
    hash_combine(hash, s.second);
  }

  return hash;
}

bool PresetSettings::matchesQuery(const Glib::ustring &query) const
{
  for(auto &s : m_settings)
    if(s.second->matchesQuery(query))
      return true;

  return false;
}
