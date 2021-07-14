#include "ModulateableParameterWithUnusualModUnit.h"
#include <xml/Writer.h>
#include <math.h>
#include <device-settings/DebugLevel.h>
#include <parameters/scale-converters/ScaleConverter.h>

ModulateableParameterWithUnusualModUnit::ModulateableParameterWithUnusualModUnit(
    ParameterGroup *group, ParameterId id, const ScaleConverter *scaling, const ScaleConverter *modAmountScaling,
    tDisplayValue def, int coarseDenominator, int fineDenominator)
    : ModulateableParameter(group, id, scaling, def, coarseDenominator, fineDenominator)
    , m_modAmountScaling(modAmountScaling)
{
}

void ModulateableParameterWithUnusualModUnit::writeDocProperties(
    Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const
{
  ModulateableParameter::writeDocProperties(writer, knownRevision);
  writer.writeTextElement("mod-amount-stringizer", m_modAmountScaling->controlPositionToDisplayJS());
}

Glib::ustring ModulateableParameterWithUnusualModUnit::stringizeModulationAmount() const
{
  return stringizeModulationAmount(getModulationAmount());
}

double ModulateableParameterWithUnusualModUnit::getModulationAmountFineDenominator() const
{
  return m_modAmountScaling->getFineDenominator(getValue());
}

double ModulateableParameterWithUnusualModUnit::getModulationAmountCoarseDenominator() const
{
  return m_modAmountScaling->getCoarseDenominator(getValue());
}

Glib::ustring ModulateableParameterWithUnusualModUnit::stringizeModulationAmount(tControlPositionValue amount) const
{
  return m_modAmountScaling->getDimension().stringize(m_modAmountScaling->controlPositionToDisplay(amount));
}
