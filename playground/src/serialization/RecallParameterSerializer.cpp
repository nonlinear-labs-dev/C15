#include "RecallParameterSerializer.h"
#include "presets/recall/RecallParameter.h"

RecallParameterSerializer::RecallParameterSerializer(RecallParameter *target)
    : Serializer(getTagName())
{
  m_parameter = target;
}

void RecallParameterSerializer::writeTagContent(Writer &writer) const
{
  writer.writeTextElement("value", to_string(m_parameter->getRecallValue()));
  writer.writeTextElement("mod-amt", to_string(m_parameter->getRecallModulationAmount()));
  writer.writeTextElement("mod-source", to_string(static_cast<int>(m_parameter->getRecallModSource())));
}

void RecallParameterSerializer::readTagContent(Reader &reader) const
{
  reader.onTextElement(
      "value", [this](const ustring &text, const Attributes &attr) { m_parameter->m_recallValue = std::stod(text); });

  reader.onTextElement("mod-amt", [this](const ustring &text, const Attributes &attr) {
    m_parameter->m_recallModAmount = std::stod(text);
  });

  reader.onTextElement("mod-source", [this](const ustring &text, const Attributes &attr) {
    m_parameter->m_recallModSource = static_cast<MacroControls>(std::stoi(text));
  });
}

Glib::ustring RecallParameterSerializer::getTagName()
{
  return "recall-parameter";
}
