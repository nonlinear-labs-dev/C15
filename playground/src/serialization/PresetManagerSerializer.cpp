#include <presets/EditBuffer.h>
#include <presets/PresetBank.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/panel-unit/boled/SplashLayout.h>
#include <serialization/EditBufferSerializer.h>
#include <serialization/PresetBankSerializer.h>
#include <serialization/PresetManagerSerializer.h>
#include <xml/Reader.h>
#include <xml/Writer.h>
#include <tools/TimeTools.h>


PresetManagerSerializer::PresetManagerSerializer(PresetManager &pm) :
    Serializer(getTagName()), m_pm(pm)
{
}

PresetManagerSerializer::~PresetManagerSerializer() {
  m_pm.onRestore();
}

Glib::ustring PresetManagerSerializer::getTagName()
{
  return "preset-manager";
}

void PresetManagerSerializer::writeTagContent(Writer &writer) const
{
  writer.writeTextElement("serialize-date", TimeTools::getAdjustedIso());
  writer.writeTextElement("selected-bank-uuid", m_pm.m_selectedBankUUID);

  size_t numBanks = m_pm.getNumBanks();

  SplashLayout::addStatus("Writing PresetManager");

  for (size_t i = 0; i < numBanks; i++)
  {
    auto bank = m_pm.getBank(i);
    PresetBankSerializer bankWriter(bank);
    bankWriter.write(writer);
  }

  EditBufferSerializer editBufferWriter(m_pm.getEditBuffer());
  editBufferWriter.write(writer);
}

void PresetManagerSerializer::readTagContent(Reader &reader) const
{
  SplashLayout::addStatus("Reading PresetManager");

  reader.onTextElement("selected-bank-uuid", [&](const Glib::ustring &text, const Attributes &attributes)
  {
    m_pm.undoableSelectBank(reader.getTransaction(), text);
  });

  reader.onTag(PresetBankSerializer::getTagName(), [&](const Attributes &attributes) mutable
  {
    int curBankPos = m_pm.getNumBanks();
    m_pm.addBank(reader.getTransaction(), false);
    return new PresetBankSerializer(m_pm.getBank (curBankPos));
  });

  reader.onTag(EditBufferSerializer::getTagName(), [&](const Attributes &attributes) mutable
  {
    return new EditBufferSerializer(m_pm.getEditBuffer());
  });


}

