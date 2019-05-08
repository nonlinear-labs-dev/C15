#include "OutputMixerGroup.h"

#include "parameters/Parameter.h"
#include "parameters/ModulateableParameter.h"

#include "parameters/scale-converters/LinearBipolar100PercentScaleConverter.h"
#include "parameters/scale-converters/LinearBipolar50DbScaleConverter.h"
#include "parameters/scale-converters/Linear100PercentScaleConverter.h"
#include "parameters/scale-converters/ParabolicGainDbScaleConverter.h"
#include "parameters/scale-converters/Linear50DbScaleConverter.h"
#include <parameters/scale-converters/LinearBipolar200PercentScaleConverter.h>
#include <parameters/ModulateableParameterWithUnusualModUnit.h>

OutputMixerGroup::OutputMixerGroup(ParameterGroupSet *parent)
    : ParameterGroup(parent, "Mixer", "Output Mixer", "Output Mixer", "Output Mixer")
{
}

OutputMixerGroup::~OutputMixerGroup()
{
}

void OutputMixerGroup::init()
{
  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 169, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 171, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 172, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 174, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 175, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 177, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 178, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 180, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 181, ScaleConverter::get<Linear50DbScaleConverter>(),
      ScaleConverter::get<LinearBipolar50DbScaleConverter>(), 0.0, 100, 500));

  appendParameter(new Parameter(this, 183, ScaleConverter::get<Linear100PercentScaleConverter>(), 0.5, 100, 1000));

  appendParameter(new Parameter(this, 184, ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(
      new ModulateableParameter(this, 185, ScaleConverter::get<ParabolicGainDbScaleConverter>(), 0.38, 100, 1000));

  appendParameter(new Parameter(this, 187, ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));
}
