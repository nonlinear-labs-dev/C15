#include "PresetBankSerializer.h"
#include "PresetManagerSerializer.h"
#include <tools/TimeTools.h>
#include <proxies/hwui/panel-unit/boled/SplashLayout.h>
#include <presets/PresetManager.h>

PresetManagerSerializer::PresetManagerSerializer(PresetManager *pm)
    : Serializer(getTagName())
    , m_pm(pm)
{
}

PresetManagerSerializer::~PresetManagerSerializer()
{
  m_pm->m_sigRestoreHappened.send();
}

Glib::ustring PresetManagerSerializer::getTagName()
{
  return "preset-manager";
}

void PresetManagerSerializer::writeTagContent(Writer &writer) const
{
  writer.writeTextElement("serialize-date", TimeTools::getAdjustedIso());
  writer.writeTextElement("selected-bank-uuid", m_pm->getSelectedBankUuid().raw());

  SplashLayout::addStatus("Writing PresetManager");

  m_pm->forEachBank([&](auto bank) {
    PresetBankSerializer bankWriter(bank);
    bankWriter.write(writer);
  });
}

void PresetManagerSerializer::readTagContent(Reader &reader) const
{
  SplashLayout::addStatus("Reading PresetManager");

  reader.onTextElement("selected-bank-uuid", [&](const auto &text, const auto &) {
    m_pm->selectBank(reader.getTransaction(), Uuid { text });
  });

  reader.onTag(PresetBankSerializer::getTagName(),
               [&](const auto &) mutable { return new PresetBankSerializer(m_pm->addBank(reader.getTransaction())); });
}
