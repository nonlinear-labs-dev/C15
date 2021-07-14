#include "PresetParameterGroupsSerializer.h"
#include "PresetParameterGroupSerializer.h"
#include "SplitGroupsFromOldGlobalGroupSerializer.h"
#include "PresetSplitGroupsFromOldGlobalGroupSerializer.h"
#include <presets/Preset.h>
#include <presets/PresetParameterGroup.h>

PresetParameterGroupsSerializer::PresetParameterGroupsSerializer(Preset* preset)
    : Serializer(getTagName())
    , m_preset(preset)
{
}

Glib::ustring PresetParameterGroupsSerializer::getTagName()
{
  return "parameter-groups";
}

void PresetParameterGroupsSerializer::writeTagContent(Writer& writer) const
{
  for(auto vg : { VoiceGroup::Global, VoiceGroup::I, VoiceGroup::II })
  {
    for(auto& paramGroup : m_preset->m_parameterGroups[static_cast<size_t>(vg)])
    {
      PresetParameterGroupSerializer group(paramGroup.second.get(), m_preset->getType());
      group.write(writer, Attribute("id", paramGroup.first));
    }
  }
}

void PresetParameterGroupsSerializer::readTagContent(Reader& reader) const
{
  reader.onTag(PresetParameterGroupSerializer::getTagName(), [&](const Attributes& attr) mutable -> PresetParameterGroupSerializer* {
    auto groupID = GroupId(attr.get("id"));

    if(groupID.getName() == "Split" && groupID.getVoiceGroup() == VoiceGroup::Global) {
      return new PresetSplitGroupsFromOldGlobalGroupSerializer(m_preset);
    } else {
      return new PresetParameterGroupSerializer(m_preset->findOrCreateParameterGroup(groupID), m_preset->getType());
    }
  });
}