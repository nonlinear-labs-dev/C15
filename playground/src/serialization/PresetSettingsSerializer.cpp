#include "xml/Attributes.h"
#include "presets/PresetSetting.h"
#include "PresetSettingsSerializer.h"
#include "xml/Writer.h"
#include "xml/Reader.h"

PresetSettingsSerializer::PresetSettingsSerializer(Preset *preset)
    : Serializer(getTagName())
    , m_preset(preset)
{
}

Glib::ustring PresetSettingsSerializer::getTagName()
{
  return "preset-settings";
}

void PresetSettingsSerializer::writeTagContent(Writer &writer) const
{
  for(const auto &a : m_preset->getSettings().getSettings())
  {
    writer.writeTextElement("setting", a.second->save(), Attribute("name", a.first));
  }
}

void PresetSettingsSerializer::readTagContent(Reader &reader) const
{
  reader.onTextElement("setting", [&](const Glib::ustring &text, const Attributes &attr) mutable {
    m_preset->getSettings().set(reader.getTransaction(), attr.get("name"), text);
  });
}
