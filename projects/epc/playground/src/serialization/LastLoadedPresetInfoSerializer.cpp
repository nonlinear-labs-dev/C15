#include "../xml/Attributes.h"
#include "LastLoadedPresetInfoSerializer.h"
#include "presets/EditBuffer.h"
#include "ParameterGroupsSerializer.h"
#include "xml/Reader.h"
#include "xml/Writer.h"

LastLoadedPresetInfoSerializer::LastLoadedPresetInfoSerializer(EditBuffer *editBuffer)
    : Serializer(getTagName())
    , m_editBuffer(editBuffer)
{
}

Glib::ustring LastLoadedPresetInfoSerializer::getTagName()
{
  return "last-loaded-preset-info";
}

void LastLoadedPresetInfoSerializer::writeTagContent(Writer &writer) const
{
  writer.writeTextElement("loaded-preset", m_editBuffer->getUUIDOfLastLoadedPreset().raw());
  writer.writeTextElement("loaded-presets-name", m_editBuffer->getName());
  writer.writeTextElement("voice-group-label-I", m_editBuffer->getVoiceGroupName(VoiceGroup::I));
  writer.writeTextElement("voice-group-label-II", m_editBuffer->getVoiceGroupName(VoiceGroup::II));

  writer.writeTextElement("origin-I", m_editBuffer->getAttribute("origin-I", ""));
  writer.writeTextElement("origin-I-vg", m_editBuffer->getAttribute("origin-I-vg", ""));
  writer.writeTextElement("origin-II", m_editBuffer->getAttribute("origin-II", ""));
  writer.writeTextElement("origin-II-vg", m_editBuffer->getAttribute("origin-II-vg", ""));
}

void LastLoadedPresetInfoSerializer::readTagContent(Reader &reader) const
{
  reader.onTextElement("loaded-preset", [&](auto txt, auto) { m_editBuffer->m_lastLoadedPreset = Uuid { txt }; });
  reader.loadTextElement("loaded-presets-name", m_editBuffer->m_name);
  reader.loadTextElement("voice-group-label-I", m_editBuffer->m_voiceGroupLabels[0]);
  reader.loadTextElement("voice-group-label-II", m_editBuffer->m_voiceGroupLabels[1]);

  reader.onTextElement("origin-I", [&](auto text, auto) mutable {
    m_editBuffer->setAttribute(reader.getTransaction(), "origin-I", text);
  });

  reader.onTextElement("origin-II", [&](auto text, auto) mutable {
    m_editBuffer->setAttribute(reader.getTransaction(), "origin-II", text);
  });

  reader.onTextElement("origin-I-vg", [&](auto text, auto) mutable {
    m_editBuffer->setAttribute(reader.getTransaction(), "origin-I-vg", text);
  });

  reader.onTextElement("origin-II-vg", [&](auto text, auto) mutable {
    m_editBuffer->setAttribute(reader.getTransaction(), "origin-II-vg", text);
  });
}
