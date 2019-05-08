#include <groups/EnvelopeCGroup.h>
#include <parameters/ModulateableParameterWithUnusualModUnit.h>
#include <parameters/scale-converters/EnvelopeAttackDecayTimeMSScaleConverter.h>
#include <parameters/scale-converters/EnvelopeReleaseTimeMSScaleConverter.h>
#include <parameters/scale-converters/EnvelopeTimeMSScaleConverter.h>
#include <parameters/scale-converters/Linear100PercentScaleConverter.h>
#include <parameters/scale-converters/Linear60DbScaleConverter.h>
#include <parameters/scale-converters/Linear60DbtScaleConverter.h>
#include <parameters/scale-converters/LinearBipolar100PercentScaleConverter.h>
#include <parameters/scale-converters/LinearBipolar1DbstScaleConverter.h>
#include <parameters/scale-converters/LinearBipolar200PercentScaleConverter.h>
#include <parameters/scale-converters/ScaleConverter.h>

EnvelopeCGroup::EnvelopeCGroup(ParameterGroupSet *parent)
    : ParameterGroup(parent, "Env C", "Envelope C", "Envelope C", "Envelope C")
{
}

EnvelopeCGroup::~EnvelopeCGroup()
{
}

void EnvelopeCGroup::init()
{
  appendParameter(new ModulateableParameter(this, 38, ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(), 0,
                                            100, 1000));

  appendParameter(new Parameter(this, 296, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameter(this, 40, ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(),
                                            0.59, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 42, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0.5, 100, 1000));

  appendParameter(new ModulateableParameter(this, 44, ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(),
                                            0.79, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 297, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(
      new ModulateableParameter(this, 46, ScaleConverter::get<EnvelopeReleaseTimeMSScaleConverter>(), 0.53, 101, 1010));

  appendParameter(new Parameter(this, 48, ScaleConverter::get<Linear60DbScaleConverter>(), 0.5, 60, 600));

  appendParameter(new Parameter(this, 49, ScaleConverter::get<Linear60DbtScaleConverter>(), 0, 60, 600));

  appendParameter(new Parameter(this, 50, ScaleConverter::get<Linear60DbtScaleConverter>(), 0, 60, 600));

  appendParameter(new Parameter(this, 51, ScaleConverter::get<LinearBipolar1DbstScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 52, ScaleConverter::get<Linear100PercentScaleConverter>(), 0.05, 100, 1000));
}
