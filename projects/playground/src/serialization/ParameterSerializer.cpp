#include "xml/Attributes.h"
#include "xml/Writer.h"
#include "xml/Reader.h"
#include "parameters/ModulateableParameter.h"
#include "parameters/MacroControlParameter.h"
#include "parameters/Parameter.h"
#include <parameters/PhysicalControlParameter.h>
#include <parameters/RibbonParameter.h>
#include <parameters/PedalParameter.h>
#include "ParameterSerializer.h"
#include "parameters/ParameterImportConversions.h"
#include <groups/ParameterGroup.h>
#include <presets/EditBuffer.h>

ParameterSerializer::ParameterSerializer(Parameter *param)
    : Serializer(getTagName())
    , m_param(param)
{
}

Glib::ustring ParameterSerializer::getTagName()
{
  return "parameter";
}

void ParameterSerializer::writeTagContent(Writer &writer) const
{
  if(m_param)
    writer.writeTextElement("value", to_string(m_param->getValue().getRawValue()));

  tryWriteModulateableParameter(writer);
  tryWriteMacroControlParameter(writer);
  tryWriteHardwareSourceParameter(writer);
}

void ParameterSerializer::tryWriteModulateableParameter(Writer &writer) const
{
  if(auto p = dynamic_cast<const ModulateableParameter *>(m_param))
  {
    writer.writeTextElement("modAmount", to_string(p->getModulationAmount()));
    writer.writeTextElement("modSrc", to_string(p->getModulationSource()));
  }
}

void ParameterSerializer::tryWriteMacroControlParameter(Writer &writer) const
{
  if(auto p = dynamic_cast<const MacroControlParameter *>(m_param))
  {
    writer.writeTextElement("givenName", p->getGivenName());
    writer.writeTextElement("info", p->getInfo());
  }
}

void ParameterSerializer::tryWriteHardwareSourceParameter(Writer &writer) const
{
  if(auto p = dynamic_cast<const PhysicalControlParameter *>(m_param))
  {
    if(auto ribbon = dynamic_cast<const RibbonParameter *>(p))
    {
      int mode = static_cast<int>(ribbon->getRibbonTouchBehaviour());
      writer.writeTextElement("ribbon-touch-behaviour", to_string(mode));

      mode = static_cast<int>(ribbon->getRibbonReturnMode());
      writer.writeTextElement("ribbon-return-mode", to_string(mode));
    }

    if(auto pedal = dynamic_cast<const PedalParameter *>(p))
    {
      int mode = static_cast<int>(pedal->getPedalMode());
      writer.writeTextElement("pedalMode", to_string(mode));
    }
  }
}

void ParameterSerializer::readTagContent(Reader &reader) const
{
  if(m_param && !m_param->isLocked())
  {
    reader.onTextElement("value", [&](const Glib::ustring &text, const Attributes &) mutable {
      auto v = std::stod(text);
      auto type = static_cast<EditBuffer *>(m_param->getParentGroup()->getParent())->getType();
      auto converted
          = ParameterImportConversions::get().convert(m_param->getID().getNumber(), v, reader.getFileVersion(), type);
      m_param->loadFromPreset(reader.getTransaction(), converted);
    });

    tryLoadModulateableParameter(reader);
    tryLoadMacroControlParameter(reader);
    tryLoadHardwareSourceParameter(reader);
  }
}

void ParameterSerializer::tryLoadModulateableParameter(Reader &reader) const
{
  if(auto p = dynamic_cast<ModulateableParameter *>(m_param))
  {
    reader.onTextElement("modAmount", [=, &reader](const Glib::ustring &text, const Attributes &) {
      auto v = std::stod(text);
      auto converted
          = ParameterImportConversions::get().convertMCAmount(m_param->getID().getNumber(), v, reader.getFileVersion());
      p->setModulationAmount(reader.getTransaction(), converted);
    });

    reader.onTextElement(
        "modSrc", [=, &reader](const Glib::ustring &text, const Attributes &) { loadModulationAmount(reader, text); });
  }
}

void ParameterSerializer::loadModulationAmount(Reader &reader, const Glib::ustring &text) const
{
  if(auto p = dynamic_cast<ModulateableParameter *>(m_param))
  {
    int i = std::stoi(text);

    if(i == -1)
      i = 0;

    auto macro = static_cast<MacroControls>(i);
    p->setModulationSource(reader.getTransaction(), macro);
  }
}

void ParameterSerializer::tryLoadMacroControlParameter(Reader &reader) const
{
  if(auto p = dynamic_cast<MacroControlParameter *>(m_param))
  {
    p->undoableSetInfo(reader.getTransaction(), "");

    reader.onTextElement("givenName", [=, &reader](const Glib::ustring &text, const Attributes &) {
      p->undoableSetGivenName(reader.getTransaction(), text);
    });

    reader.onTextElement("info", [=, &reader](const Glib::ustring &text, const Attributes &) {
      p->undoableSetInfo(reader.getTransaction(), text);
    });
  }
}

void ParameterSerializer::tryLoadHardwareSourceParameter(Reader &reader) const
{
  if(auto p = dynamic_cast<PhysicalControlParameter *>(m_param))
  {
    if(auto ribbon = dynamic_cast<RibbonParameter *>(p))
    {
      reader.onTextElement("ribbon-touch-behaviour", [=, &reader](const Glib::ustring &text, const Attributes &) {
        int mode = std::stoi(text);
        ribbon->undoableSetRibbonTouchBehaviour(reader.getTransaction(), static_cast<RibbonTouchBehaviour>(mode));
      });

      reader.onTextElement("ribbon-return-mode", [=, &reader](const Glib::ustring &text, const Attributes &) {
        int mode = std::stoi(text);
        ribbon->undoableSetRibbonReturnMode(reader.getTransaction(), static_cast<RibbonReturnMode>(mode));
      });
    }

    if(auto pedal = dynamic_cast<PedalParameter *>(p))
    {
      reader.onTextElement("pedalMode", [=, &reader](const Glib::ustring &text, const Attributes &) {
        int mode = std::stoi(text);
        pedal->undoableSetPedalMode(reader.getTransaction(), static_cast<PedalModes>(mode));
      });
    }
  }
}
