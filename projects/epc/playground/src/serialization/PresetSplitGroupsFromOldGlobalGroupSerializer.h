#pragma once
#include <serialization/PresetParameterGroupSerializer.h>
#include <presets/Preset.h>

class PresetSplitGroupsFromOldGlobalGroupSerializer : public PresetParameterGroupSerializer
{
 public:
  explicit PresetSplitGroupsFromOldGlobalGroupSerializer(Preset* preset);
  void readTagContent(Reader& reader) const override;

 private:
  Preset* m_preset;
  PresetParameterGroup* m_vgII;
};
