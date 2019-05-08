#include "xml/Attributes.h"
#include "EditBufferSerializer.h"
#include "xml/Writer.h"
#include "xml/Reader.h"
#include "presets/PresetManager.h"
#include <presets/PresetBank.h>
#include "PresetOrderSerializer.h"
#include <device-settings/DebugLevel.h>

PresetOrderSerializer::PresetOrderSerializer(shared_ptr<PresetBank> bank, bool ignoreUUIDs)
    : Serializer(getTagName())
    , m_bank(bank)
    , m_ignoreUUIDs(ignoreUUIDs)
{
}

Glib::ustring PresetOrderSerializer::getTagName()
{
  return "preset-order";
}

void PresetOrderSerializer::writeTagContent(Writer &writer) const
{
  for(auto p : m_bank->m_presets)
  {
    writer.writeTextElement("uuid", p->getUuid());
  }
}

void PresetOrderSerializer::readTagContent(Reader &reader) const
{
  reader.onTextElement("uuid", [&](const Glib::ustring &text, const Attributes &attributes) mutable {
    Uuid uuid = m_ignoreUUIDs ? Uuid() : Uuid(text);
    m_bank->undoableAppendPreset(reader.getTransaction(), uuid);
  });
}
