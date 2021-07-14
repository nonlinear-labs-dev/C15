#include "PresetBankMetadataSerializer.h"
#include "PresetOrderSerializer.h"
#include "AttributesOwnerSerializer.h"
#include <presets/Bank.h>
#include <proxies/hwui/panel-unit/boled/SplashLayout.h>
#include <giomm/file.h>

PresetBankMetadataSerializer::PresetBankMetadataSerializer(Bank *bank, bool ignoreUUIDs)
    : Serializer(getTagName())
    , m_bank(bank)
    , m_ignoreUUIDs(ignoreUUIDs)
{
}

Glib::ustring PresetBankMetadataSerializer::getTagName()
{
  return "bank";
}

void PresetBankMetadataSerializer::writeTagContent(Writer &writer) const
{
  writer.writeTextElement("name", m_bank->getName(false));
  writer.writeTextElement("uuid", m_bank->getUuid().raw());
  writer.writeTextElement("x", m_bank->getX());
  writer.writeTextElement("y", m_bank->getY());
  writer.writeTextElement("selected-preset", m_bank->getSelectedPresetUuid().raw());
  writer.writeTextElement("attached-to-bank", m_bank->getAttachedToBankUuid().raw());
  writer.writeTextElement("attach-direction", m_bank->getAttachDirection());

  PresetOrderSerializer presetOrder(m_bank, m_ignoreUUIDs);
  presetOrder.write(writer);

  AttributesOwnerSerializer attributesWriter(m_bank);
  attributesWriter.write(writer);

  writer.writeTextElement("last-changed-timestamp", to_string(m_bank->m_lastChangedTimestamp));
}

void PresetBankMetadataSerializer::readTagContent(Reader &reader) const
{
  reader.onTextElement("name", [&](const Glib::ustring &text, const Attributes &) {
    m_bank->setName(reader.getTransaction(), text);

    if(!text.empty())
      SplashLayout::addStatus("Reading bank " + text);
  });

  if(!m_ignoreUUIDs)
  {
    reader.onTextElement("uuid", [&](const Glib::ustring &text, const Attributes &) {
      m_bank->setUuid(reader.getTransaction(), Uuid { text });
    });
  }

  reader.onTextElement(
      "x", [&](const Glib::ustring &text, const Attributes &) { m_bank->setX(reader.getTransaction(), text); });

  reader.onTextElement(
      "y", [&](const Glib::ustring &text, const Attributes &) { m_bank->setY(reader.getTransaction(), text); });

  reader.onTextElement("selected-preset", [&](const Glib::ustring &text, const Attributes &) {
    m_bank->selectPreset(reader.getTransaction(), Uuid { text });
  });

  reader.onTextElement("attached-to-bank", [&](const Glib::ustring &text, const Attributes &) {
    m_bank->setAttachedToBank(reader.getTransaction(), Uuid { text });
  });

  reader.onTextElement("attach-direction", [&](const Glib::ustring &text, const Attributes &) {
    m_bank->setAttachedDirection(reader.getTransaction(), text);
  });

  reader.onTag(PresetOrderSerializer::getTagName(),
               [&](const Attributes &) mutable { return new PresetOrderSerializer(m_bank, m_ignoreUUIDs); });

  reader.onTag(AttributesOwnerSerializer::getTagName(),
               [&](const Attributes &) mutable { return new AttributesOwnerSerializer(m_bank); });

  reader.onTextElement("last-changed-timestamp", [&](auto text, auto) {
    reader.getTransaction()->addPostfixCommand(
        [text = text, bank = m_bank](auto) { bank->m_lastChangedTimestamp = std::stoull(text); });
  });
}
