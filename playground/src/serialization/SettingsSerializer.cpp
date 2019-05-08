#include "xml/Attributes.h"
#include "xml/Writer.h"
#include "xml/Reader.h"
#include "device-settings/Setting.h"
#include "SettingsSerializer.h"

SettingsSerializer::SettingsSerializer(Settings &settings)
    : Serializer(getTagName())
    , m_settings(settings)
{
}

Glib::ustring SettingsSerializer::getTagName()
{
  return "settings";
}

void SettingsSerializer::writeTagContent(Writer &writer) const
{
  for(const auto &a : m_settings.getSettings())
  {
    writer.writeTextElement("setting", a.second->save(), Attribute("id", a.first));
  }
}

void SettingsSerializer::readTagContent(Reader &reader) const
{
  reader.onTextElement("setting", [&](const Glib::ustring &text, const Attributes &attr) mutable {
    if(auto s = m_settings.getSetting(attr.get("id")))
      s->load(text);
  });
}
