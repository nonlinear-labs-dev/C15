#include "ReverbGroup.h"
#include "parameters/Parameter.h"
#include "parameters/ModulateableParameter.h"

#include "parameters/scale-converters/Linear100PercentScaleConverter.h"

ReverbGroup::ReverbGroup(ParameterGroupSet *parent, VoiceGroup vg)
    : ParameterGroup(parent, { "Reverb", vg }, "Reverb", "Reverb", "Reverb")
{
}

ReverbGroup::~ReverbGroup()
{
}

void ReverbGroup::init()
{
  appendParameter(new ModulateableParameter(this, { 235, getVoiceGroup() },
                                            ScaleConverter::get<Linear100PercentScaleConverter>(), 0.33, 100, 1000));

  appendParameter(new Parameter(this, { 237, getVoiceGroup() }, ScaleConverter::get<Linear100PercentScaleConverter>(),
                                0.33, 100, 1000));

  appendParameter(new ModulateableParameter(this, { 238, getVoiceGroup() },
                                            ScaleConverter::get<Linear100PercentScaleConverter>(), 0.5, 100, 1000));

  appendParameter(new Parameter(this, { 240, getVoiceGroup() }, ScaleConverter::get<Linear100PercentScaleConverter>(),
                                0.25, 100, 1000));

  appendParameter(new ModulateableParameter(this, { 241, getVoiceGroup() },
                                            ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameter(this, { 344, getVoiceGroup() },
                                            ScaleConverter::get<Linear100PercentScaleConverter>(), 1.0, 100, 1000));
}
