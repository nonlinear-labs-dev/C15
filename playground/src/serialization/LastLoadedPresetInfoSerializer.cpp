#include "../xml/Attributes.h"
#include "LastLoadedPresetInfoSerializer.h"
#include "presets/EditBuffer.h"
#include "ParameterGroupsSerializer.h"
#include "xml/Reader.h"
#include "xml/Writer.h"

LastLoadedPresetInfoSerializer::LastLoadedPresetInfoSerializer(LastLoadedPresetInfo &info)
    : Serializer(getTagName())
    , m_info(info)
{
}

LastLoadedPresetInfoSerializer::~LastLoadedPresetInfoSerializer()
{
}

Glib::ustring LastLoadedPresetInfoSerializer::getTagName()
{
  return "last-loaded-preset-info";
}

void LastLoadedPresetInfoSerializer::writeTagContent(Writer &writer) const
{
  writer.writeTextElement("loaded-preset", m_info.presetUUID);
  writer.writeTextElement("loaded-bank", m_info.bankUUID);

  writer.writeTextElement("loaded-presets-name", m_info.presetName);
  writer.writeTextElement("loaded-presets-bank-name", m_info.bankName);

  writer.writeTextElement("preset-was-deleted", to_string(m_info.presetDeleted));
}

void LastLoadedPresetInfoSerializer::readTagContent(Reader &reader) const
{
  reader.loadTextElement("loaded-preset", m_info.presetUUID);
  reader.loadTextElement("loaded-bank", m_info.bankUUID);

  reader.loadTextElement("loaded-presets-name", m_info.presetName);
  reader.loadTextElement("loaded-presets-bank-name", m_info.bankName);

  reader.loadElement("preset-was-deleted", m_info.presetDeleted);
}
