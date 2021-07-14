#include "../xml/Attributes.h"
#include "EditBufferSerializer.h"
#include <serialization/ParameterGroupsSerializer.h>
#include "LastLoadedPresetInfoSerializer.h"
#include "presets/EditBuffer.h"
#include "xml/Reader.h"
#include "xml/Writer.h"
#include "PresetParameterGroupsSerializer.h"
#include "ParameterGroupSerializer.h"
#include <proxies/hwui/panel-unit/boled/SplashLayout.h>
#include "serialization/RecallEditBufferSerializer.h"
#include "VoiceGroupsLockSerializer.h"
#include "SplitGroupsFromOldGlobalGroupSerializer.h"
#include <groups/ParameterGroup.h>
#include <nltools/logging/Log.h>

EditBufferSerializer::EditBufferSerializer(EditBuffer *editBuffer)
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

  for(auto vg : { VoiceGroup::Global, VoiceGroup::I, VoiceGroup::II })
  {
    for(auto group : m_editBuffer->getParameterGroups(vg))
    {
      ParameterGroupSerializer s(group);
      s.write(writer, Attribute { "id", group->getID().toString() });
    }
  }

  writer.writeTextElement("selected-parameter-number", to_string(m_editBuffer->getSelectedParameterNumber()));

  LastLoadedPresetInfoSerializer lastLoaded(m_editBuffer);
  lastLoaded.write(writer);

  writer.writeTextElement("hash-on-store", to_string(m_editBuffer->m_hashOnStore));

  VoiceGroupsLockSerializer lockSerializer(m_editBuffer);
  lockSerializer.write(writer);

  writer.writeTextElement("editbuffer-type", toString(m_editBuffer->getType()));

  RecallEditBufferSerializer recall(m_editBuffer);
  recall.write(writer);
}

bool isOldSplitGroup(const GroupId &id)
{
  return id.getVoiceGroup() == VoiceGroup::Global && id.getName() == "Split";
}

void EditBufferSerializer::readTagContent(Reader &reader) const
{
  SplashLayout::addStatus("Reading Edit Buffer");

  reader.onTextElement("editbuffer-type", [&](auto text, auto) mutable {
    m_editBuffer->undoableSetType(reader.getTransaction(), to<SoundType>(text));
  });

  reader.onTag(ParameterGroupSerializer::getTagName(), [&](auto attr) -> ParameterGroupSerializer * {
    auto id = attr.get("id");
    auto groupId = GroupId(id);
    auto group = m_editBuffer->getParameterGroupByID(groupId);

    if(!group)
    {

      if(isOldSplitGroup(groupId))
      {
        nltools::Log::warning("Encountered:", id,
                              "which was split into dual groups! (initializing them to the same values now)");

        return new SplitGroupsFromOldGlobalGroupSerializer(m_editBuffer);
      }

      nltools::Log::error("Could not find group:", id, "which is parsed to:", GroupId(id));

      return nullptr;
    }

    return new ParameterGroupSerializer(group);
  });

  reader.onTextElement("selected-parameter-number", [&](auto text, auto) mutable {
    try
    {
      auto num = std::stoi(text);
      ParameterId id { num, ParameterId::isGlobal(num) ? VoiceGroup::Global : VoiceGroup::I };
      m_editBuffer->undoableSelectParameter(reader.getTransaction(), id, false);
    }
    catch(const std::invalid_argument &err)
    {
      nltools::Log::error(err.what(), "Selecting default Parameter..");
      m_editBuffer->undoableSelectParameter(reader.getTransaction(), { 0, VoiceGroup::I }, false);
    }
  });

  reader.onTag(LastLoadedPresetInfoSerializer::getTagName(),
               [&](auto) mutable { return new LastLoadedPresetInfoSerializer(m_editBuffer); });

  reader.onTextElement("hash-on-store", [&](auto text, auto) {
    std::stringstream ss(text);
    ss >> m_editBuffer->m_hashOnStore;
    m_editBuffer->onChange();
  });

  reader.onTag(VoiceGroupsLockSerializer::getTagName(),
               [&](auto) mutable { return new VoiceGroupsLockSerializer(m_editBuffer); });

  reader.onTextElement("locked-parameter", [&](auto text, auto) mutable {
#warning "TODO: locked parameters per voice group?"
    //m_editBuffer->findParameterByID(std::stoi(text))->undoableLock(reader.getTransaction());
  });

  reader.onTag(RecallEditBufferSerializer::getTagName(),
               [&](auto) mutable { return new RecallEditBufferSerializer(m_editBuffer); });
}
