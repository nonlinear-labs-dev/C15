#include "xml/Attributes.h"
#include "EditBufferSerializer.h"
#include "xml/Writer.h"
#include "xml/Reader.h"
#include "presets/PresetManager.h"
#include <presets/PresetBank.h>
#include "PresetBankOrderSerializer.h"
#include <device-settings/DebugLevel.h>

PresetBankOrderSerializer::PresetBankOrderSerializer(PresetManager &pm)
    : Serializer(getTagName())
    , m_pm(pm)
{
}

Glib::ustring PresetBankOrderSerializer::getTagName()
{
  return "bank-order";
}

void PresetBankOrderSerializer::writeTagContent(Writer &writer) const
{
  for(auto b : m_pm.m_banks)
  {
    writer.writeTextElement("uuid", b->getUuid());
  }
}

void PresetBankOrderSerializer::readTagContent(Reader &reader) const
{
  reader.onTextElement("uuid", [&](const Glib::ustring &text, const Attributes &attributes) mutable {
    Uuid uuid(text);
    m_pm.undoableAppendBank(reader.getTransaction(), uuid, false);
  });
}
