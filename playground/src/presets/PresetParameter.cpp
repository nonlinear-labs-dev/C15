#include "PresetParameter.h"
#include "PresetParameterGroup.h"
#include <libundo/undo/Transaction.h>
#include <assert.h>
#include <parameters/Parameter.h>
#include <xml/Writer.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/scale-converters/LinearBipolar100PercentScaleConverter.h>

PresetParameter::PresetParameter(int id)
    : m_id{ id }
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
  assert(m_id == other->m_id);
  transaction->addUndoSwap(eb, m_value, other->m_value);
  transaction->addUndoSwap(eb, m_fields, other->m_fields);
}

void PresetParameter::copyFrom(UNDO::Transaction *transaction, const ::Parameter *other)
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  assert(m_id == other->getID());
  transaction->addUndoSwap(eb, m_fields, {});
  other->copyTo(transaction, this);
}

void PresetParameter::writeDiff(Writer &writer, int parameterID, const PresetParameter *other) const
{
  auto ebParam = Application::get().getPresetManager()->getEditBuffer()->findParameterByID(parameterID);

  writer.writeTag("parameter", Attribute("name", ebParam->getLongName()), [&] {
    auto sc = ebParam->getValue().getScaleConverter();
    auto myString = sc->getDimension().stringize(sc->controlPositionToDisplay(m_value));
    auto otherString = sc->getDimension().stringize(sc->controlPositionToDisplay(other->m_value));

    if(myString != otherString)
      writer.writeTextElement("value", "", Attribute("a", myString), Attribute("b", otherString));

    if(getModulationSource() != other->getModulationSource())
    {
      writer.writeTextElement("mc-select", "", Attribute("a", getModulationSource()),
                              Attribute("b", other->getModulationSource()));
    }

    if(getModulationAmount() != other->getModulationAmount())
    {
      auto c = ScaleConverter::get<LinearBipolar100PercentScaleConverter>();
      auto currentParameter = c->getDimension().stringize(c->controlPositionToDisplay(getModulationAmount()));
      auto otherParameter = c->getDimension().stringize(c->controlPositionToDisplay(other->getModulationAmount()));
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
      writer.writeTextElement("return-mode", "", Attribute("a", (int) getRibbonReturnMode()),
                              Attribute("b", (int) other->getRibbonReturnMode()));
    }

    if(getRibbonTouchBehaviour() != other->getRibbonTouchBehaviour())
    {
      writer.writeTextElement("behaviour", "", Attribute("a", getRibbonTouchBehaviour()),
                              Attribute("b", other->getRibbonTouchBehaviour()));
    }

    if(getPedalMode() != other->getPedalMode())
    {
      writer.writeTextElement("return-mode", "", Attribute("a", (int) getPedalMode()),
                              Attribute("b", (int) other->getPedalMode()));
    }
  });
}

ModulationSource PresetParameter::getModulationSource() const
{
  auto it = m_fields.find(Fields::ModSource);

  if(it != m_fields.end() && !it->second.empty())
    return static_cast<ModulationSource>(stoi(it->second));

  return ModulationSource::NONE;
}

double PresetParameter::getModulationAmount() const
{
  auto it = m_fields.find(Fields::ModAmount);

  if(it != m_fields.end() && !it->second.empty())
    return stod(it->second);

  return 0.0;
}

string PresetParameter::getGivenName() const
{
  auto it = m_fields.find(Fields::GivenName);

  if(it != m_fields.end())
    return it->second;

  return "";
}

string PresetParameter::getInfo() const
{
  auto it = m_fields.find(Fields::Info);

  if(it != m_fields.end())
    return it->second;

  return "";
}

const int PresetParameter::getID() const
{
  return m_id;
}

enum RibbonReturnMode PresetParameter::getRibbonReturnMode() const
{
  auto it = m_fields.find(Fields::RibbonReturnMode);

  if(it != m_fields.end() && !it->second.empty())
    return static_cast<enum RibbonReturnMode>(stoi(it->second));

  using RRM = enum RibbonReturnMode;
  return RRM::RETURN;
}

enum RibbonTouchBehaviour PresetParameter::getRibbonTouchBehaviour() const
{
  auto it = m_fields.find(Fields::RibbonTouchBehaviour);

  if(it != m_fields.end() && !it->second.empty())
    return static_cast<enum RibbonTouchBehaviour>(stoi(it->second));

  using RTB = enum RibbonTouchBehaviour;
  return RTB::ABSOLUTE;
}

enum PedalModes PresetParameter::getPedalMode() const
{
  auto it = m_fields.find(Fields::PedalMode);

  if(it != m_fields.end() && !it->second.empty())
    return static_cast<enum PedalModes>(stoi(it->second));

  return PedalModes::STAY;
}

void PresetParameter::writeDocument(Writer &writer) const
{
  writer.writeTag("param",
                  { Attribute{ "id", to_string(m_id) }, Attribute{ "value", to_string(m_value) },
                    Attribute{ "mod-src", to_string(static_cast<int>(getModulationSource())) },
                    Attribute{ "mod-amt", to_string(getModulationAmount()) } },
                  []() {});
}

string paramFieldToString(PresetParameter::Fields f)
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
