#include <parameters/ModulateableParameter.h>
#include <parameters/ModulateableParameterWithUnusualModUnit.h>
#include <parameters/scale-converters/LinearBipolar96StScaleConverterFine.h>
#include "MasterGroup.h"
#include "parameters/Parameter.h"

#include "parameters/scale-converters/LinearBipolar48StScaleConverter.h"
#include "parameters/scale-converters/ParabolicGainDbScaleConverter.h"

MasterGroup::MasterGroup(ParameterDualGroupSet *parent)
    : ParameterGroup(parent, { "Master", VoiceGroup::Global }, "Master", "Master", "Master")
{
}

MasterGroup::~MasterGroup()
{
}

void MasterGroup::init()
{
  appendParameter(new ModulateableParameter(this, { 247, VoiceGroup::Global },
                                            ScaleConverter::get<ParabolicGainDbScaleConverter>(), 0.5, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, { 248, VoiceGroup::Global }, ScaleConverter::get<LinearBipolar48StScaleConverter>(),
      ScaleConverter::get<LinearBipolar96StScaleConverterFine>(), 0, 48, 4800));
}

void MasterGroup::undoableRandomize(UNDO::Transaction *transaction, Initiator initiator, double amount)
{
}
