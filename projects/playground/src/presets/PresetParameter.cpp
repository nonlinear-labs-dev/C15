#include "PresetParameter.h"
#include "PresetParameterGroup.h"
#include <libundo/undo/Transaction.h>
#include <nltools/Assert.h>
#include <parameters/Parameter.h>
#include <xml/Writer.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/scale-converters/LinearBipolar100PercentScaleConverter.h>
#include <groups/ParameterGroup.h>
#include <libundo/undo/Scope.h>
#include <xml/Attribute.h>
#include <parameter_declarations.h>
#include <parameters/SplitPointParameter.h>
#include <parameters/scale-converters/LinearCountScaleConverter.h>
#include <parameters/scale-converters/dimension/VoicesDimension.h>

PresetParameter::PresetParameter(const ParameterId &id)
    : m_id { id }
{
}

PresetParameter::PresetParameter(const ::Parameter &other)
    : PresetParameter(other.getID())
{
  auto trash = UNDO::Scope::startTrashTransaction();
  other.copyTo(trash->getTransaction(), this);
}

PresetParameter::PresetParameter(const PresetParameter &other)
    : PresetParameter(other.m_id)
{
  m_value = other.m_value;
  m_fields = other.m_fields;
}

tControlPositionValue PresetParameter::getValue() const
{
  return m_value;
}

void PresetParameter::setValue(UNDO::Transaction *transaction, tControlPositionValue v)
{
  v = ScaleConverter::getControlPositionRangeBipolar().clip(v);
  transaction->addUndoSwap(m_value, v);
}

void PresetParameter::setField(UNDO::Transaction *transaction, Fields field, const std::string &value)
{
  transaction->addSimpleCommand([this, field, s = UNDO::createSwapData(value)](auto) {
    auto &v = m_fields[field];
    s->swapWith(v);
  });
}

void PresetParameter::copyFrom(UNDO::Transaction *transaction, const PresetParameter *other)
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  nltools_assertAlways(m_id == other->m_id);
  transaction->addUndoSwap(eb, m_value, other->m_value);
  transaction->addUndoSwap(eb, m_fields, other->m_fields);
}

void PresetParameter::copyFrom(UNDO::Transaction *transaction, const ::Parameter *other)
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  nltools_assertAlways(m_id == other->getID());
  transaction->addUndoSwap(eb, m_fields, {});
  other->copyTo(transaction, this);
}

void PresetParameter::writeDiff(Writer &writer, ParameterId parameterID, const PresetParameter *other) const
{
  auto ebParam = Application::get().getPresetManager()->getEditBuffer()->findParameterByID(parameterID);

  bool aFound = ebParam != nullptr;
  bool bFound = other != nullptr;
  auto name = aFound ? std::string(ebParam->getLongName()) : getID().toString();

  writer.writeTag(
      "parameter", Attribute("name", name), Attribute("afound", aFound ? "true" : "false"),
      Attribute("bfound", bFound ? "true" : "false"), Attribute("id-num", getID().getNumber()), [&] {
        if(aFound && bFound)
        {
          auto sc = ebParam->getValue().getScaleConverter();
          auto myString = sc->getDimension().stringize(sc->controlPositionToDisplay(m_value));
          auto otherString = sc->getDimension().stringize(sc->controlPositionToDisplay(other->m_value));

          if(ebParam->getID() == ParameterId { C15::PID::Split_Split_Point, VoiceGroup::Global })
          {
            if(auto split = dynamic_cast<SplitPointParameter *>(ebParam))
            {
              myString = split->getDisplayString(VoiceGroup::I, m_value);
              otherString = split->getDisplayString(VoiceGroup::I, other->m_value);
            }
          }
          else if(ebParam->getID().getNumber() == C15::PID::Unison_Voices)
          {
            //we always use the single converter as unison voices get saved in 24 voices format
            auto singleConverter = ScaleConverter::get<LinearCountScaleConverter<24, VoicesDimension>>();

            auto stringize
                = [](auto sc, auto v) { return sc->getDimension().stringize(sc->controlPositionToDisplay(v)); };

            myString = stringize(singleConverter, m_value);
            otherString = stringize(singleConverter, other->m_value);
          }

          if(myString != otherString)
            writer.writeTextElement("value", "", Attribute("a", myString), Attribute("b", otherString));

          if(getModulationSource() != other->getModulationSource())
          {
            writer.writeTextElement("mc-select", "", Attribute("a", getModulationSource()),
                                    Attribute("b", other->getModulationSource()));
          }

          if(differs(getModulationAmount(), other->getModulationAmount()))
          {
            auto c = ScaleConverter::get<LinearBipolar100PercentScaleConverter>();
            auto currentParameter = c->getDimension().stringize(c->controlPositionToDisplay(getModulationAmount()));
            auto otherParameter
                = c->getDimension().stringize(c->controlPositionToDisplay(other->getModulationAmount()));
            writer.writeTextElement("mc-amount", "", Attribute("a", currentParameter), Attribute("b", otherParameter));
          }

          if(getGivenName() != other->getGivenName())
          {
            writer.writeTextElement("name", "", Attribute("a", getGivenName()), Attribute("b", other->getGivenName()));
          }

          if(getInfo() != other->getInfo())
          {
            writer.writeTextElement("info", "", Attribute("a", "changed"), Attribute("b", "changed"));
          }

          if(getRibbonReturnMode() != other->getRibbonReturnMode())
          {
            writer.writeTextElement("return-mode", "", Attribute("a", static_cast<int>(getRibbonReturnMode())),
                                    Attribute("b", static_cast<int>(other->getRibbonReturnMode())));
          }

          if(getRibbonTouchBehaviour() != other->getRibbonTouchBehaviour())
          {
            writer.writeTextElement("behaviour", "", Attribute("a", getRibbonTouchBehaviour()),
                                    Attribute("b", other->getRibbonTouchBehaviour()));
          }

          if(getPedalMode() != other->getPedalMode())
          {
            writer.writeTextElement("return-mode", "", Attribute("a", static_cast<int>(getPedalMode())),
                                    Attribute("b", static_cast<int>(other->getPedalMode())));
          }
        }
      });
}

MacroControls PresetParameter::getModulationSource() const
{
  auto it = m_fields.find(Fields::ModSource);

  if(it != m_fields.end() && !it->second.empty())
    return static_cast<MacroControls>(std::stoi(it->second));

  return MacroControls::NONE;
}

double PresetParameter::getModulationAmount() const
{
  auto it = m_fields.find(Fields::ModAmount);

  if(it != m_fields.end() && !it->second.empty())
    return std::stod(it->second);

  return 0.0;
}

std::string PresetParameter::getGivenName() const
{
  auto it = m_fields.find(Fields::GivenName);

  if(it != m_fields.end())
    return it->second;

  return "";
}

std::string PresetParameter::getInfo() const
{
  auto it = m_fields.find(Fields::Info);

  if(it != m_fields.end())
    return it->second;

  return "";
}

const ParameterId PresetParameter::getID() const
{
  return m_id;
}

enum RibbonReturnMode PresetParameter::getRibbonReturnMode() const
{
  auto it = m_fields.find(Fields::RibbonReturnMode);

  if(it != m_fields.end() && !it->second.empty())
    return static_cast<enum RibbonReturnMode>(std::stoi(it->second));

  using RRM = enum RibbonReturnMode;
  return RRM::RETURN;
}

enum RibbonTouchBehaviour PresetParameter::getRibbonTouchBehaviour() const
{
  auto it = m_fields.find(Fields::RibbonTouchBehaviour);

  if(it != m_fields.end() && !it->second.empty())
    return static_cast<enum RibbonTouchBehaviour>(std::stoi(it->second));

  using RTB = enum RibbonTouchBehaviour;
  return RTB::ABSOLUTE;
}

enum PedalModes PresetParameter::getPedalMode() const
{
  auto it = m_fields.find(Fields::PedalMode);

  if(it != m_fields.end() && !it->second.empty())
    return static_cast<enum PedalModes>(std::stoi(it->second));

  return PedalModes::STAY;
}

void PresetParameter::writeDocument(Writer &writer) const
{
  writer.writeTag("param",
                  { Attribute { "id", m_id.toString() }, Attribute { "value", to_string(m_value) },
                    Attribute { "mod-src", to_string(static_cast<int>(getModulationSource())) },
                    Attribute { "mod-amt", to_string(getModulationAmount()) } },
                  []() {});
}

std::string paramFieldToString(PresetParameter::Fields f)
{
  switch(f)
  {
    case PresetParameter::Fields::ModAmount:
      return "modAmount";

    case PresetParameter::Fields::ModSource:
      return "modSrc";

    case PresetParameter::Fields::GivenName:
      return "givenName";

    case PresetParameter::Fields::Info:
      return "info";

    case PresetParameter::Fields::RibbonTouchBehaviour:
      return "ribbon-touch-behaviour";

    case PresetParameter::Fields::RibbonReturnMode:
      return "ribbon-return-mode";

    case PresetParameter::Fields::PedalMode:
      return "pedalMode";
  }
  return "";
}
