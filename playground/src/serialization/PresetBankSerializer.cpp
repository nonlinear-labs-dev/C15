#include "xml/Attributes.h"
#include "xml/Writer.h"
#include "xml/Reader.h"
#include "presets/PresetBank.h"
#include "PresetBankSerializer.h"
#include "PresetSerializer.h"
#include <proxies/hwui/panel-unit/boled/SplashLayout.h>

PresetBankSerializer::PresetBankSerializer(shared_ptr<PresetBank> bank, bool ignoreUUIDs)
    : super(bank, ignoreUUIDs)
{
}

void PresetBankSerializer::writeTagContent(Writer &writer) const
{
  SplashLayout::addStatus("Writing bank " + m_bank->getName(true));

  super::writeTagContent(writer);

  size_t numPresets = m_bank->getNumPresets();

  for(size_t i = 0; i < numPresets; i++)
  {
    PresetSerializer presetWriter(m_bank->getPreset(i).get());
    presetWriter.write(writer, Attribute("pos", i));
  }
}

void PresetBankSerializer::readTagContent(Reader &reader) const
{
  super::readTagContent(reader);

  reader.onTag(PresetSerializer::getTagName(), [&](const Attributes &attr) mutable {
    size_t pos = stoul(attr.get("pos"));

    while(m_bank->getNumPresets() <= pos)
      m_bank->undoableInsertPreset(reader.getTransaction(), m_bank->getNumPresets());

    return new PresetSerializer(m_bank->getPreset(pos).get(), m_ignoreUUIDs);
  });
}
