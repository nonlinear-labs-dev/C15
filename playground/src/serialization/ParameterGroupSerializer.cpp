#include "xml/Attributes.h"
#include "xml/Writer.h"
#include "xml/Reader.h"
#include "groups/ParameterGroup.h"
#include "parameters/Parameter.h"
#include "ParameterGroupSerializer.h"
#include "ParameterSerializer.h"

ParameterGroupSerializer::ParameterGroupSerializer(ParameterGroup *paramGroup)
    : Serializer(getTagName())
    , m_paramGroup(paramGroup)
{
  for(auto p : m_paramGroup->getParameters())
    m_parameterById[p->getID()] = p;
}

ParameterGroupSerializer::~ParameterGroupSerializer()
= default;

Glib::ustring ParameterGroupSerializer::getTagName()
{
  return "parameter-group";
}

void ParameterGroupSerializer::writeTagContent(Writer &writer) const
{
  for(auto param : m_paramGroup->getParameters())
  {
    ParameterSerializer paramSerializer(param);
    paramSerializer.write(writer, Attribute("id", param->getID()));
  }
}

void ParameterGroupSerializer::readTagContent(Reader &reader) const
{
  if(m_paramGroup)
  {
    for(auto p : m_paramGroup->getParameters())
    {
      p->removeFlag(ParameterFlags::Loaded);
    }

    reader.onTag(ParameterSerializer::getTagName(), [&](auto attr) mutable {
      auto p = m_parameterById.find(std::stoi(attr.get("id")));
      if(p != m_parameterById.end())
      {
        auto param = p->second;
        param->setFlag(ParameterFlags::Loaded);
        return new ParameterSerializer(param);
      }
      return static_cast<ParameterSerializer *>(nullptr);
    });
  }
}
