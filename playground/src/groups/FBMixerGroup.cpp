#include "FBMixerGroup.h"
#include "parameters/Parameter.h"
#include "parameters/ModulateableParameter.h"

#include "parameters/scale-converters/LinearBipolar100PercentScaleConverter.h"
#include "parameters/scale-converters/Linear100PercentScaleConverter.h"
#include "parameters/scale-converters/Linear50DbScaleConverter.h"
#include "parameters/scale-converters/LinearBipolar50DbScaleConverter.h"
#include "parameters/scale-converters/ParabolicGainDbScaleConverter.h"
#include "parameters/scale-converters/LinearBipolar70DbScaleConverter.h"
#include <parameters/scale-converters/LinearBipolar200PercentScaleConverter.h>
#include <parameters/scale-converters/LinearBipolar1DbstScaleConverter.h>
#include <parameters/ModulateableParameterWithUnusualModUnit.h>
#include <parameters/scale-converters/Linear70DbScaleConverter.h>

FBMixerGroup::FBMixerGroup(ParameterDualGroupSet *parent, VoiceGroup vg)
    : ParameterGroup(parent, { "FB", vg }, "FB Mixer", "Feedback Mixer", "Feedback Mixer")
{
}

FBMixerGroup::~FBMixerGroup()
{
}

void FBMixerGroup::init()
{
  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, { 156, getVoiceGroup() }, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, { 158, getVoiceGroup() }, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, { 160, getVoiceGroup() }, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameter(this, { 162, getVoiceGroup() },
                                            ScaleConverter::get<Linear100PercentScaleConverter>(), 0.5, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, { 164, getVoiceGroup() }, ScaleConverter::get<Linear70DbScaleConverter>(),
      ScaleConverter::get<LinearBipolar70DbScaleConverter>(), (2.0 / 7.0), 140, 700));

  appendParameter(new Parameter(this, { 166, getVoiceGroup() }, ScaleConverter::get<Linear100PercentScaleConverter>(),
                                0.5, 100, 1000));

  appendParameter(new Parameter(this, { 167, getVoiceGroup() }, ScaleConverter::get<Linear100PercentScaleConverter>(),
                                0, 100, 1000));

  appendParameter(new Parameter(this, { 168, getVoiceGroup() }, ScaleConverter::get<LinearBipolar1DbstScaleConverter>(),
                                0, 100, 1000));

  appendParameter(new ModulateableParameter(this, { 299, getVoiceGroup() },
                                            ScaleConverter::get<ParabolicGainDbScaleConverter>(), 0.38, 100, 1000));

  appendParameter(new ModulateableParameter(
      this, { 346, getVoiceGroup() }, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(), 0.0, 100, 1000));

  appendParameter(new ModulateableParameter(this, { 348, getVoiceGroup() },
                                            ScaleConverter::get<Linear100PercentScaleConverter>(), 0.0, 100, 1000));

  appendParameter(new ModulateableParameter(this, { 350, getVoiceGroup() },
                                            ScaleConverter::get<Linear100PercentScaleConverter>(), 0.0, 100, 1000));

  appendParameter(new ModulateableParameter(this, { 352, getVoiceGroup() },
                                            ScaleConverter::get<Linear100PercentScaleConverter>(), 0.0, 100, 1000));

  appendParameter(new ModulateableParameter(this, { 354, getVoiceGroup() },
                                            ScaleConverter::get<Linear100PercentScaleConverter>(), 0.0, 100, 1000));
}
