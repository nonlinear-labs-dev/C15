#include "../xml/Attributes.h"
#include "EditBufferSerializer.h"
#include "LastLoadedPresetInfoSerializer.h"
#include "presets/EditBuffer.h"
#include "PresetSerializer.h"
#include "xml/Reader.h"
#include "xml/Writer.h"
#include <proxies/hwui/panel-unit/boled/SplashLayout.h>

EditBufferSerializer::EditBufferSerializer(shared_ptr<EditBuffer> editBuffer)
    : Serializer(getTagName())
    , m_editBuffer(editBuffer)
{
}

EditBufferSerializer::~EditBufferSerializer()
{
}

Glib::ustring EditBufferSerializer::getTagName()
{
  return "edit-buffer";
}

void EditBufferSerializer::writeTagContent(Writer &writer) const
{
  SplashLayout::addStatus("Writing Edit Buffer");

  PresetSerializer groups(m_editBuffer.get());
  groups.write(writer);

  if(auto selectedParam = m_editBuffer->getSelectedParameter())
    writer.writeTextElement("selected-parameter", to_string(selectedParam->getID()));

  LastLoadedPresetInfoSerializer lastLoaded(m_editBuffer->m_lastLoadedPresetInfo);
  lastLoaded.write(writer);

  writer.writeTextElement("hash-on-store", to_string(m_editBuffer->m_hashOnStore));

  for(auto g : m_editBuffer->getParameterGroups())
  {
    for(auto p : g->getParameters())
    {
      if(p->isLocked())
      {
        writer.writeTextElement("locked-parameter", to_string(p->getID()));
      }
    }
  }
}

void EditBufferSerializer::readTagContent(Reader &reader) const
{
  SplashLayout::addStatus("Reading Edit Buffer");

  for(auto g : m_editBuffer->getParameterGroups())
  {
    for(auto p : g->getParameters())
    {
      if(p->isLocked())
      {
        p->undoableUnlock(reader.getTransaction());
      }
    }
  }

  reader.onTag(PresetSerializer::getTagName(),
               [&](const Attributes &attr) mutable { return new PresetSerializer(m_editBuffer.get()); });

  reader.onTextElement("selected-parameter", [&](const Glib::ustring &text, const Attributes &attr) mutable {
    m_editBuffer->undoableSelectParameter(reader.getTransaction(), text);
  });

  reader.onTag(LastLoadedPresetInfoSerializer::getTagName(), [&](const Attributes &attr) mutable {
    return new LastLoadedPresetInfoSerializer(m_editBuffer->m_lastLoadedPresetInfo);
  });

  reader.loadElement("hash-on-store", m_editBuffer->m_hashOnStore);

  reader.onTextElement("locked-parameter", [&](const Glib::ustring &text, const Attributes &attr) mutable {
    m_editBuffer->findParameterByID(stoi(text))->undoableLock(reader.getTransaction());
  });
}
