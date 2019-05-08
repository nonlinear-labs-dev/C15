#include <groups/EnvelopeAGroup.h>
#include <parameters/ModulateableParameterWithUnusualModUnit.h>
#include <parameters/scale-converters/EnvelopeAttackDecayTimeMSScaleConverter.h>
#include <parameters/scale-converters/EnvelopeReleaseTimeMSScaleConverter.h>
#include <parameters/scale-converters/Linear100PercentScaleConverter.h>
#include <parameters/scale-converters/Linear60DbScaleConverter.h>
#include <parameters/scale-converters/Linear60DbtScaleConverter.h>
#include <parameters/scale-converters/LinearBipolar100PercentScaleConverter.h>
#include <parameters/scale-converters/LinearBipolar1DbstScaleConverter.h>
#include <parameters/scale-converters/LinearBipolar24DbScaleConverter.h>
#include <parameters/scale-converters/LinearBipolar48DbScaleConverter.h>
#include <parameters/scale-converters/ScaleConverter.h>
#include <parameters/ModulateableParameterWithUnusualModDenominator.h>

EnvelopeAGroup::EnvelopeAGroup(ParameterGroupSet *parent)
    : ParameterGroup(parent, "Env A", "Envelope A", "Envelope A", "Envelope A")
{
}

EnvelopeAGroup::~EnvelopeAGroup()
{
}

void EnvelopeAGroup::init()
{
  appendParameter(
      new ModulateableParameter(this, 0, ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 294, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameter(this, 2, ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(),
                                            0.59, 100, 1000));

  appendParameter(
      new ModulateableParameter(this, 4, ScaleConverter::get<Linear100PercentScaleConverter>(), 0.5, 100, 1000));

  appendParameter(new ModulateableParameter(this, 6, ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(),
                                            0.79, 100, 1000));

  appendParameter(
      new ModulateableParameter(this, 8, ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModDenominator(
      this, 10, ScaleConverter::get<EnvelopeReleaseTimeMSScaleConverter>(), 0.53, 101, 1010, 100, 1000));

  appendParameter(
      new ModulateableParameterWithUnusualModUnit(this, 12, ScaleConverter::get<LinearBipolar24DbScaleConverter>(),
                                                  ScaleConverter::get<LinearBipolar48DbScaleConverter>(), 0, 48, 480));

  appendParameter(new Parameter(this, 14, ScaleConverter::get<Linear60DbScaleConverter>(), 0.5, 60, 600));

  appendParameter(new Parameter(this, 15, ScaleConverter::get<Linear60DbtScaleConverter>(), 0, 60, 600));

  appendParameter(new Parameter(this, 16, ScaleConverter::get<Linear60DbtScaleConverter>(), 0, 60, 600));

  appendParameter(new Parameter(this, 17, ScaleConverter::get<LinearBipolar1DbstScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 18, ScaleConverter::get<Linear100PercentScaleConverter>(), 0.05, 100, 1000));
}
