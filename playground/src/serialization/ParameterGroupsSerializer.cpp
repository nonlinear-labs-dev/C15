#include "xml/Attributes.h"
#include "xml/Writer.h"
#include "xml/Reader.h"
#include "presets/ParameterGroupSet.h"
#include "ParameterGroupsSerializer.h"
#include "ParameterGroupSerializer.h"

ParameterGroupsSerializer::ParameterGroupsSerializer(ParameterGroupSet *paramGroups)
    : Serializer(getTagName())
    , m_paramGroups(paramGroups)
{
}

Glib::ustring ParameterGroupsSerializer::getTagName()
{
  return "parameter-groups";
}

void ParameterGroupsSerializer::writeTagContent(Writer &writer) const
{
  for(auto paramGroup : m_paramGroups->getParameterGroups())
  {
    ParameterGroupSerializer group(paramGroup);
    group.write(writer, Attribute("id", paramGroup->getID()));
  }
}

void ParameterGroupsSerializer::readTagContent(Reader &reader) const
{
  reader.onTag(ParameterGroupSerializer::getTagName(), [&](const Attributes &attr) mutable {
    auto group = m_paramGroups->getParameterGroupByID(attr.get("id"));
    return new ParameterGroupSerializer(group);
  });
}
