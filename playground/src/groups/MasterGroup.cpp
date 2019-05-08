#include "MasterGroup.h"
#include "parameters/Parameter.h"
#include "parameters/ModulateableParameter.h"

#include "parameters/scale-converters/LinearBipolar48StScaleConverter.h"
#include "parameters/scale-converters/ParabolicGainDbScaleConverter.h"

MasterGroup::MasterGroup(ParameterGroupSet *parent)
    : ParameterGroup(parent, "Master", "Master", "Master", "Master")
{
}

MasterGroup::~MasterGroup()
{
}

void MasterGroup::init()
{
  appendParameter(new Parameter(this, 247, ScaleConverter::get<ParabolicGainDbScaleConverter>(), 0.5, 100, 1000));

  appendParameter(new Parameter(this, 248, ScaleConverter::get<LinearBipolar48StScaleConverter>(), 0, 48, 4800));
}

void MasterGroup::undoableRandomize(UNDO::Scope::tTransactionPtr transaction, Initiator initiator, double amount)
{
}
