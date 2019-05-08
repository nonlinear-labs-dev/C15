#include "FBMixerGroup.h"
#include "parameters/Parameter.h"
#include "parameters/ModulateableParameter.h"

#include "parameters/scale-converters/LinearBipolar100PercentScaleConverter.h"
#include "parameters/scale-converters/Linear100PercentScaleConverter.h"
#include "parameters/scale-converters/Linear50DbScaleConverter.h"
#include "parameters/scale-converters/LinearBipolar50DbScaleConverter.h"
#include "parameters/scale-converters/ParabolicGainDbScaleConverter.h"
#include <parameters/scale-converters/LinearBipolar200PercentScaleConverter.h>
#include <parameters/scale-converters/LinearBipolar1DbstScaleConverter.h>
#include <parameters/ModulateableParameterWithUnusualModUnit.h>

FBMixerGroup::FBMixerGroup(ParameterGroupSet *parent)
    : ParameterGroup(parent, "FB", "FB Mixer", "Feedback Mixer", "Feedback Mixer")
{
}

FBMixerGroup::~FBMixerGroup()
{
}

void FBMixerGroup::init()
{
  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 156, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 158, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 160, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(
      new ModulateableParameter(this, 162, ScaleConverter::get<Linear100PercentScaleConverter>(), 0.5, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 164, ScaleConverter::get<Linear50DbScaleConverter>(),
      ScaleConverter::get<LinearBipolar50DbScaleConverter>(), 0.0, 100, 500));

  appendParameter(new Parameter(this, 166, ScaleConverter::get<Linear100PercentScaleConverter>(), 0.5, 100, 1000));

  appendParameter(new Parameter(this, 167, ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 168, ScaleConverter::get<LinearBipolar1DbstScaleConverter>(), 0, 100, 1000));

  appendParameter(
      new ModulateableParameter(this, 299, ScaleConverter::get<ParabolicGainDbScaleConverter>(), 0.38, 100, 1000));
}
