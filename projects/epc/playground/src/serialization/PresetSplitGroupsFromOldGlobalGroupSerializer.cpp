#include "PresetSplitGroupsFromOldGlobalGroupSerializer.h"

PresetSplitGroupsFromOldGlobalGroupSerializer::PresetSplitGroupsFromOldGlobalGroupSerializer(Preset *preset)
    : PresetParameterGroupSerializer { preset->findParameterGroup({ "Split", VoiceGroup::I }), preset->getType() }
    , m_vgII { preset->findParameterGroup({ "Split", VoiceGroup::II }) }
    , m_preset { preset }
{
}

void PresetSplitGroupsFromOldGlobalGroupSerializer::readTagContent(Reader &reader) const
{
  PresetParameterGroupSerializer::readTagContent(reader);
  m_vgII->copyFrom(reader.getTransaction(), m_paramGroup);
}
