#include <serialization/AttributesOwnerSerializer.h>
#include "xml/Attributes.h"
#include "presets/Preset.h"
#include "ParameterGroupsSerializer.h"
#include "PresetSerializer.h"
#include "PresetSettingsSerializer.h"
#include "xml/Writer.h"
#include "xml/Reader.h"

PresetSerializer::PresetSerializer(Preset *preset, bool ignoreUUIDs)
    : Serializer(getTagName())
    , m_preset(preset)
    , m_ignoreUUIDs(ignoreUUIDs)
{
}

PresetSerializer::~PresetSerializer()
{
}

Glib::ustring PresetSerializer::getTagName()
{
  return "preset";
}

void PresetSerializer::writeTagContent(Writer &writer) const
{
  writer.writeTextElement("name", m_preset->getName());
  writer.writeTextElement("uuid", m_preset->getUuid());

  AttributesOwnerSerializer attributesWriter(m_preset);
  attributesWriter.write(writer);

  PresetSettingsSerializer settingsWriter(m_preset);
  settingsWriter.write(writer);

  ParameterGroupsSerializer groups(m_preset);
  groups.write(writer);
}

void PresetSerializer::readTagContent(Reader &reader) const
{
  reader.onTextElement("name", [&](const Glib::ustring &text, const Attributes &attr) {
    m_preset->undoableSetName(reader.getTransaction(), text);
  });

  if(!m_ignoreUUIDs)
  {
    reader.onTextElement("uuid", [&](const Glib::ustring &text, const Attributes &attr) {
      m_preset->undoableSetUuid(reader.getTransaction(), text);
    });
  }

  reader.onTag(AttributesOwnerSerializer::getTagName(),
               [&](const Attributes &attr) mutable { return new AttributesOwnerSerializer(m_preset); });

  reader.onTag(PresetSettingsSerializer::getTagName(),
               [&](const Attributes &attr) mutable { return new PresetSettingsSerializer(m_preset); });

  reader.onTag(ParameterGroupsSerializer::getTagName(),
               [&](const Attributes &attr) mutable { return new ParameterGroupsSerializer(m_preset); });
}
