#include "AttributesOwnerSerializer.h"
#include "PresetSerializer.h"
#include "PresetParameterGroupsSerializer.h"
#include <presets/Preset.h>
#include <nltools/logging/Log.h>

PresetSerializer::PresetSerializer(Preset *preset, bool ignoreUUIDs)
    : Serializer(getTagName())
    , m_preset(preset)
    , m_presetLock(preset)
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
  writer.writeTextElement("uuid", m_preset->getUuid().raw());
  writer.writeTextElement("type", toString(m_preset->getType()));
  writer.writeTextElement("vg-I-name", m_preset->getVoiceGroupName(VoiceGroup::I));
  writer.writeTextElement("vg-II-name", m_preset->getVoiceGroupName(VoiceGroup::II));

  AttributesOwnerSerializer attributesWriter(m_preset);
  attributesWriter.write(writer);

  PresetParameterGroupsSerializer groups(m_preset);
  groups.write(writer);
}

void PresetSerializer::readTagContent(Reader &reader) const
{
  reader.onTextElement("name", [&](auto &text, auto) { m_preset->setName(reader.getTransaction(), text); });

  reader.onTextElement("type",
                       [&](auto &text, auto) { m_preset->setType(reader.getTransaction(), to<SoundType>(text)); });

  reader.onTextElement("vg-I-name", [&](auto &text, auto) {
    m_preset->undoableSetVoiceGroupName(reader.getTransaction(), VoiceGroup::I, text);
  });

  reader.onTextElement("vg-II-name", [&](auto &text, auto) {
    m_preset->undoableSetVoiceGroupName(reader.getTransaction(), VoiceGroup::II, text);
  });

  if(!m_ignoreUUIDs)
  {
    reader.onTextElement("uuid", [&](const Glib::ustring &text, const Attributes &) {
      m_preset->setUuid(reader.getTransaction(), Uuid { text });
    });
  }

  reader.onTag(AttributesOwnerSerializer::getTagName(),
               [&](const Attributes &) mutable { return new AttributesOwnerSerializer(m_preset); });

  reader.onTag(PresetParameterGroupsSerializer::getTagName(),
               [&](const Attributes &) mutable { return new PresetParameterGroupsSerializer(m_preset); });
}
