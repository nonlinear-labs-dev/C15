#include "PresetParameterSerializer.h"
#include <presets/PresetParameter.h>
#include <parameters/ParameterImportConversions.h>
#include <parameters/ModulateableParameter.h>
#include <parameters/RibbonParameter.h>
#include <parameters/PedalParameter.h>

PresetParameterSerializer::PresetParameterSerializer(PresetParameter *param, SoundType type)
    : Serializer(getTagName())
    , m_param(param)
    , m_type(type)
{
}

Glib::ustring PresetParameterSerializer::getTagName()
{
  return "parameter";
}

void PresetParameterSerializer::writeTagContent(Writer &writer) const
{
  writer.writeTextElement("value", to_string(m_param->m_value));

  for(auto &f : m_param->m_fields)
    writer.writeTextElement(paramFieldToString(f.first), f.second);
}

void PresetParameterSerializer::readTagContent(Reader &reader) const
{
  if(m_param)
  {
    reader.onTextElement("value", [&](const Glib::ustring &text, const Attributes &attr) mutable {
      auto v = std::stod(text);
      auto converted
          = ParameterImportConversions::get().convert(m_param->m_id.getNumber(), v, reader.getFileVersion(), m_type);
      m_param->setValue(reader.getTransaction(), converted);
    });

    reader.onTextElement("modAmount", [=, &reader](const Glib::ustring &text, const Attributes &attr) {
      double value = 0;
      if(!text.empty())
      {
        auto v = std::stod(text);
        value
            = ParameterImportConversions::get().convertMCAmount(m_param->m_id.getNumber(), v, reader.getFileVersion());
      }
      m_param->setField(reader.getTransaction(), PresetParameter::Fields::ModAmount, to_string(value));
    });

    reader.onTextElement("modSrc", [=, &reader](const Glib::ustring &text, const Attributes &attr) {
      m_param->setField(reader.getTransaction(), PresetParameter::Fields::ModSource, text);
    });

    reader.onTextElement("givenName", [=, &reader](const Glib::ustring &text, const Attributes &attr) {
      m_param->setField(reader.getTransaction(), PresetParameter::Fields::GivenName, text);
    });

    reader.onTextElement("info", [=, &reader](const Glib::ustring &text, const Attributes &attr) {
      m_param->setField(reader.getTransaction(), PresetParameter::Fields::Info, text);
    });

    reader.onTextElement("ribbon-touch-behaviour", [=, &reader](const Glib::ustring &text, const Attributes &attr) {
      m_param->setField(reader.getTransaction(), PresetParameter::Fields::RibbonTouchBehaviour, text);
    });

    reader.onTextElement("ribbon-return-mode", [=, &reader](const Glib::ustring &text, const Attributes &attr) {
      m_param->setField(reader.getTransaction(), PresetParameter::Fields::RibbonReturnMode, text);
    });

    reader.onTextElement("pedalMode", [=, &reader](const Glib::ustring &text, const Attributes &attr) {
      m_param->setField(reader.getTransaction(), PresetParameter::Fields::PedalMode, text);
    });
  }
}
