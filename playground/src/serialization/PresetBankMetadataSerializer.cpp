#include "xml/Attributes.h"
#include "xml/Writer.h"
#include "xml/Reader.h"
#include "presets/PresetBank.h"
#include "PresetBankMetadataSerializer.h"
#include "PresetSerializer.h"
#include "PresetOrderSerializer.h"
#include <proxies/hwui/panel-unit/boled/SplashLayout.h>
#include "AttributesOwnerSerializer.h"

PresetBankMetadataSerializer::PresetBankMetadataSerializer(shared_ptr<PresetBank> bank, bool ignoreUUIDs)
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
  writer.writeTextElement("uuid", m_bank->getUuid());
  writer.writeTextElement("x", m_bank->getX());
  writer.writeTextElement("y", m_bank->getY());
  writer.writeTextElement("selected-preset", m_bank->getSelectedPreset());
  writer.writeTextElement("attached-to-bank", m_bank->getAttached().uuid);
  writer.writeTextElement("attach-direction", std::to_string(m_bank->getAttached().direction));

  PresetOrderSerializer presetOrder(m_bank, m_ignoreUUIDs);
  presetOrder.write(writer);

  AttributesOwnerSerializer attributesWriter(m_bank.get());
  attributesWriter.write(writer);
}

void PresetBankMetadataSerializer::readTagContent(Reader &reader) const
{
  reader.onTextElement("name", [&](const Glib::ustring &text, const Attributes &attr) {
    m_bank->setName(reader.getTransaction(), text);
    SplashLayout::addStatus("Reading bank " + text);
  });

  if(!m_ignoreUUIDs)
  {
    reader.onTextElement("uuid", [&](const Glib::ustring &text, const Attributes &attr) {
      m_bank->setUuid(reader.getTransaction(), text);
    });
  }

  reader.onTextElement(
      "x", [&](const Glib::ustring &text, const Attributes &attr) { m_bank->setX(reader.getTransaction(), text); });

  reader.onTextElement(
      "y", [&](const Glib::ustring &text, const Attributes &attr) { m_bank->setY(reader.getTransaction(), text); });

  reader.onTextElement("selected-preset", [&](const Glib::ustring &text, const Attributes &attr) {
    m_bank->undoableSelectPreset(reader.getTransaction(), text);
  });

  reader.onTextElement("attached-to-bank", [&](const Glib::ustring &text, const Attributes &attr) {
    m_bank->setAttachedTo(reader.getTransaction(), text);
  });

  reader.onTextElement("attach-direction", [&](const Glib::ustring &text, const Attributes &attr) {
    auto dir = (PresetBank::AttachmentDirection) atoi(text.c_str());
    m_bank->setAttachedDirection(reader.getTransaction(), dir);
  });

  reader.onTag(PresetOrderSerializer::getTagName(),
               [&](const Attributes &attributes) mutable { return new PresetOrderSerializer(m_bank, m_ignoreUUIDs); });

  reader.onTag(AttributesOwnerSerializer::getTagName(),
               [&](const Attributes &attr) mutable { return new AttributesOwnerSerializer(m_bank.get()); });
}
