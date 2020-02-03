#include "PresetParameterGroupSerializer.h"
#include "PresetParameterSerializer.h"
#include <presets/PresetParameterGroup.h>
#include <presets/PresetParameter.h>

PresetParameterGroupSerializer::PresetParameterGroupSerializer(PresetParameterGroup *paramGroup)
    : Serializer(getTagName())
    , m_paramGroup(paramGroup)
{
}

PresetParameterGroupSerializer::~PresetParameterGroupSerializer()
{
}

Glib::ustring PresetParameterGroupSerializer::getTagName()
{
  return "parameter-group";
}

void PresetParameterGroupSerializer::writeTagContent(Writer &writer) const
{
  for(auto &param : m_paramGroup->m_parameters)
  {
    PresetParameterSerializer paramSerializer(param.second.get());
    paramSerializer.write(writer, Attribute("id", param.first));
  }
}

void PresetParameterGroupSerializer::readTagContent(Reader &reader) const
{
  if(m_paramGroup)
  {
    reader.onTag(PresetParameterSerializer::getTagName(), [&](const Attributes &attr) mutable {
      try
      {
        auto id = std::stoi(attr.get("id"));
        auto param = std::make_unique<PresetParameter>(id);
        auto serializer = new PresetParameterSerializer(param.get());
        m_paramGroup->m_parameters[id] = std::move(param);
        return serializer;
      }
      catch(...)
      {
        return static_cast<PresetParameterSerializer *>(nullptr);
      }
    });
  }
}
