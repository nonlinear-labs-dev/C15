#include "ModulateableParameterWithUnusualModUnit.h"
#include <xml/Writer.h>
#include <math.h>

ModulateableParameterWithUnusualModUnit::ModulateableParameterWithUnusualModUnit(
    ParameterGroup *group, uint16_t id, const ScaleConverter *scaling, const ScaleConverter *modAmountScaling,
    tDisplayValue def, int coarseDenominator, int fineDenominator)
    : ModulateableParameter(group, id, scaling, def, coarseDenominator, fineDenominator)
    , m_modAmountScaling(modAmountScaling)
{
}

void ModulateableParameterWithUnusualModUnit::undoableIncrementMCAmount(UNDO::Scope::tTransactionPtr transaction,
                                                                        int inc, ButtonModifiers modifiers)
{
  double denominator
      = modifiers[ButtonModifier::FINE] ? getModulationAmountFineDenominator() : getModulationAmountCoarseDenominator();

  tDisplayValue controlVal = getModulationAmount();
  int rasterized = round(controlVal * denominator);
  controlVal = ScaleConverter::getControlPositionRangeBipolar().clip((rasterized + inc) / denominator);
  setModulationAmount(transaction, controlVal);
}

void ModulateableParameterWithUnusualModUnit::writeDocProperties(
    Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const
{
  ModulateableParameter::writeDocProperties(writer, knownRevision);

  writer.writeTextElement("mod-amount-stringizer", m_modAmountScaling->controlPositionToDisplayJS());
  writer.writeTextElement("mod-amount-coarse", to_string(m_modAmountScaling->getCoarseDenominator(getValue())));
  writer.writeTextElement("mod-amount-fine", to_string(m_modAmountScaling->getFineDenominator(getValue())));
}

Glib::ustring ModulateableParameterWithUnusualModUnit::stringizeModulationAmount() const
{
  return m_modAmountScaling->getDimension().stringize(
      m_modAmountScaling->controlPositionToDisplay(getModulationAmount()));
}

double ModulateableParameterWithUnusualModUnit::getModulationAmountFineDenominator() const
{
  return m_modAmountScaling->getFineDenominator(getValue());
}

double ModulateableParameterWithUnusualModUnit::getModulationAmountCoarseDenominator() const
{
  return m_modAmountScaling->getCoarseDenominator(getValue());
}
