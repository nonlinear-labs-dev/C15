#include "PresetSetting.h"
#include "PresetSettings.h"
#include "xml/Writer.h"

PresetSetting::PresetSetting(PresetSettings &parent)
    : super(&parent)
{
}

PresetSetting::~PresetSetting()
{
}

connection PresetSetting::onSettingChange(slot<void, const PresetSetting *> slot)
{
  return m_signal.connectAndInit(slot, this);
}

PresetSetting::tUpdateID PresetSetting::onChange(uint64_t flags)
{
  m_signal.send(this);
  return super::onChange(flags);
}

void PresetSetting::writeDocument(Writer &writer, tUpdateID knownRevision) const
{
  bool changed = knownRevision < getUpdateIDOfLastChange();
  writer.writeTextElement("value", save(), Attribute("changed", changed));
}

bool PresetSetting::matchesQuery(const Glib::ustring &query) const
{
  return save().lowercase().find(query) != Glib::ustring::npos;
}
