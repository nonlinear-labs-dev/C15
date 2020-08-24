#include <presets/EditBuffer.h>
#include <nltools/logging/Log.h>
#include "RecallEditBufferSerializer.h"
#include "PresetParameterGroupSerializer.h"
#include <presets/recall/RecallParameter.h>

RecallEditBufferSerializer::RecallEditBufferSerializer(EditBuffer *edit)
    : Serializer(getTagName())
    , m_editBuffer(edit)
{
}

Glib::ustring RecallEditBufferSerializer::getTagName()
{
  return "recall-parameters";
}

void RecallEditBufferSerializer::writeTagContent(Writer &writer) const
{
  auto &parameters = m_editBuffer->getRecallParameterSet().getParameters();
  for(auto &param : parameters)
  {
    auto parameter = param.second.get();

    writer.writeTextElement("recall-param", to_string(parameter->getRecallValue()),
                            { Attribute("id", parameter->getID().toString()),
                              Attribute("mod-amt", to_string(parameter->getRecallModulationAmount())),
                              Attribute("mod-src", to_string(parameter->getRecallModSource())),
                              Attribute("name", parameter->getGivenName()), Attribute("info", parameter->getInfo()) });
  }
}

void RecallEditBufferSerializer::readTagContent(Reader &reader) const
{
  reader.onTextElement("recall-param", [=](const Glib::ustring &text, const Attributes &attr) {
    auto id = ParameterId(attr.get("id"));
    auto &rps = m_editBuffer->getRecallParameterSet();
    if(auto param = rps.findParameterByID(id))
    {
      param->m_recallValue = std::stod(text);
      param->m_recallModAmount = std::stod(attr.get("mod-amt"));
      param->m_recallModSource = static_cast<MacroControls>(std::stoi(attr.get("mod-src")));
      param->m_givenName = attr.get("name");
      param->m_info = attr.get("info");
    }
  });
}
