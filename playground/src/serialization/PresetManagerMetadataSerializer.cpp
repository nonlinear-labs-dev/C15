#include "xml/Attributes.h"
#include "EditBufferSerializer.h"
#include "xml/Writer.h"
#include "xml/Reader.h"
#include "presets/PresetManager.h"
#include "PresetManagerMetadataSerializer.h"
#include "PresetBankOrderSerializer.h"
#include <device-settings/DebugLevel.h>

PresetManagerMetadataSerializer::PresetManagerMetadataSerializer(PresetManager &pm)
    : Serializer(getTagName())
    , m_pm(pm)
{
}

Glib::ustring PresetManagerMetadataSerializer::getTagName()
{
  return "preset-manager";
}

void PresetManagerMetadataSerializer::writeTagContent(Writer &writer) const
{
  writer.writeTextElement("selected-bank-uuid", m_pm.m_selectedBankUUID);

  EditBufferSerializer editBufferWriter(m_pm.getEditBuffer());
  editBufferWriter.write(writer);

  PresetBankOrderSerializer bankOrder(m_pm);
  bankOrder.write(writer);
}

void PresetManagerMetadataSerializer::readTagContent(Reader &reader) const
{
  reader.onTextElement("selected-bank-uuid", [&](const Glib::ustring &text, const Attributes &attributes) {
    m_pm.undoableSelectBank(reader.getTransaction(), text);
  });

  reader.onTag(EditBufferSerializer::getTagName(),
               [&](const Attributes &attributes) mutable { return new EditBufferSerializer(m_pm.getEditBuffer()); });

  reader.onTag(PresetBankOrderSerializer::getTagName(),
               [&](const Attributes &attributes) mutable { return new PresetBankOrderSerializer(m_pm); });
}
