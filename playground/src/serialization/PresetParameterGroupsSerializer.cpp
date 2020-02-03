#include "PresetParameterGroupsSerializer.h"
#include "PresetParameterGroupSerializer.h"
#include <presets/Preset.h>
#include <presets/PresetParameterGroup.h>

PresetParameterGroupsSerializer::PresetParameterGroupsSerializer(Preset *preset)
    : Serializer(getTagName())
    , m_preset(preset)
{
}

Glib::ustring PresetParameterGroupsSerializer::getTagName()
{
  return "parameter-groups";
}

void PresetParameterGroupsSerializer::writeTagContent(Writer &writer) const
{
  for(auto &paramGroup : m_preset->m_parameterGroups)
  {
    PresetParameterGroupSerializer group(paramGroup.second.get());
    group.write(writer, Attribute("id", paramGroup.first));
  }
}

void PresetParameterGroupsSerializer::readTagContent(Reader &reader) const
{
  reader.onTag(PresetParameterGroupSerializer::getTagName(), [&](const Attributes &attr) mutable {
    try
    {
      auto id = attr.get("id");
      auto group = std::make_unique<PresetParameterGroup>();
      auto serializer = new PresetParameterGroupSerializer(group.get());
      m_preset->m_parameterGroups[id] = std::move(group);
      return serializer;
    }
    catch(...)
    {
      return static_cast<PresetParameterGroupSerializer *>(nullptr);
    }
  });
}
