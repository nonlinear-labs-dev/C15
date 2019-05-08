#include "ReverbGroup.h"
#include "parameters/Parameter.h"
#include "parameters/ModulateableParameter.h"

#include "parameters/scale-converters/Linear100PercentScaleConverter.h"

ReverbGroup::ReverbGroup(ParameterGroupSet *parent)
    : ParameterGroup(parent, "Reverb", "Reverb", "Reverb", "Reverb")
{
}

ReverbGroup::~ReverbGroup()
{
}

void ReverbGroup::init()
{
  appendParameter(
      new ModulateableParameter(this, 235, ScaleConverter::get<Linear100PercentScaleConverter>(), 0.33, 100, 1000));

  appendParameter(new Parameter(this, 237, ScaleConverter::get<Linear100PercentScaleConverter>(), 0.33, 100, 1000));

  appendParameter(
      new ModulateableParameter(this, 238, ScaleConverter::get<Linear100PercentScaleConverter>(), 0.5, 100, 1000));

  appendParameter(new Parameter(this, 240, ScaleConverter::get<Linear100PercentScaleConverter>(), 0.25, 100, 1000));

  appendParameter(
      new ModulateableParameter(this, 241, ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));
}
