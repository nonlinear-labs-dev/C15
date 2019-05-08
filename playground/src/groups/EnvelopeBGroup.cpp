#include <groups/EnvelopeBGroup.h>
#include <parameters/ModulateableParameterWithUnusualModUnit.h>
#include <parameters/scale-converters/EnvelopeAttackDecayTimeMSScaleConverter.h>
#include <parameters/scale-converters/EnvelopeReleaseTimeMSScaleConverter.h>
#include <parameters/scale-converters/EnvelopeTimeMSScaleConverter.h>
#include <parameters/scale-converters/Linear100PercentScaleConverter.h>
#include <parameters/scale-converters/Linear60DbScaleConverter.h>
#include <parameters/scale-converters/Linear60DbtScaleConverter.h>
#include <parameters/scale-converters/LinearBipolar100PercentScaleConverter.h>
#include <parameters/scale-converters/LinearBipolar1DbstScaleConverter.h>
#include <parameters/scale-converters/LinearBipolar24DbScaleConverter.h>
#include <parameters/scale-converters/LinearBipolar48DbScaleConverter.h>
#include <parameters/scale-converters/ScaleConverter.h>

EnvelopeBGroup::EnvelopeBGroup(ParameterGroupSet *parent)
    : ParameterGroup(parent, "Env B", "Envelope B", "Envelope B", "Envelope B")
{
}

EnvelopeBGroup::~EnvelopeBGroup()
{
}

void EnvelopeBGroup::init()
{
  appendParameter(new ModulateableParameter(this, 19, ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(), 0,
                                            100, 1000));

  appendParameter(new Parameter(this, 295, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameter(this, 21, ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(),
                                            0.59, 100, 1000));

  appendParameter(
      new ModulateableParameter(this, 23, ScaleConverter::get<Linear100PercentScaleConverter>(), 0.5, 100, 1000));

  appendParameter(new ModulateableParameter(this, 25, ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(),
                                            0.79, 100, 1000));

  appendParameter(
      new ModulateableParameter(this, 27, ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(
      new ModulateableParameter(this, 29, ScaleConverter::get<EnvelopeReleaseTimeMSScaleConverter>(), 0.53, 101, 1010));

  appendParameter(
      new ModulateableParameterWithUnusualModUnit(this, 31, ScaleConverter::get<LinearBipolar24DbScaleConverter>(),
                                                  ScaleConverter::get<LinearBipolar48DbScaleConverter>(), 0, 48, 480));

  appendParameter(new Parameter(this, 33, ScaleConverter::get<Linear60DbScaleConverter>(), 0.5, 60, 600));

  appendParameter(new Parameter(this, 34, ScaleConverter::get<Linear60DbtScaleConverter>(), 0, 60, 600));

  appendParameter(new Parameter(this, 35, ScaleConverter::get<Linear60DbtScaleConverter>(), 0, 60, 600));

  appendParameter(new Parameter(this, 36, ScaleConverter::get<LinearBipolar1DbstScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 37, ScaleConverter::get<Linear100PercentScaleConverter>(), 0.05, 100, 1000));
}
