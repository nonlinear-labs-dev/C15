#include <groups/FlangerGroup.h>
#include <parameters/ModulateableParameterWithUnusualModUnit.h>
#include <parameters/scale-converters/BipolarParabolic100PercentScaleConverter.h>
#include <parameters/scale-converters/Linear0To140StScaleConverter.h>
#include <parameters/scale-converters/Linear100PercentScaleConverter.h>
#include <parameters/scale-converters/Linear180DegreeScaleConverter.h>
#include <parameters/scale-converters/Linear60To140StScaleConverter.h>
#include <parameters/scale-converters/LinearBipolar100PercentScaleConverter.h>
#include <parameters/scale-converters/LinearBipolar200PercentScaleConverter.h>
#include <parameters/scale-converters/LinearBipolar50PercentScaleConverter.h>
#include <parameters/scale-converters/Parabolic10HzScaleConverter.h>
#include <parameters/scale-converters/Parabolic50MsScaleConverter.h>
#include <parameters/scale-converters/ScaleConverter.h>

FlangerGroup::FlangerGroup(ParameterGroupSet *parent)
    : ParameterGroup(parent, "Flang", "Flanger", "Flanger", "Flanger")
{
}

FlangerGroup::~FlangerGroup()
{
}

void FlangerGroup::init()
{
  appendParameter(new ModulateableParameter(this, 211, ScaleConverter::get<BipolarParabolic100PercentScaleConverter>(),
                                            0, 100, 1000));

  appendParameter(new Parameter(this, 213, ScaleConverter::get<Linear180DegreeScaleConverter>(), 0.5, 180, 1800));

  appendParameter(
      new ModulateableParameter(this, 214, ScaleConverter::get<Parabolic10HzScaleConverter>(), 0.317, 100, 1000));

  appendParameter(
      new ModulateableParameter(this, 216, ScaleConverter::get<Parabolic50MsScaleConverter>(), 0.317, 125, 1250));

  appendParameter(new Parameter(this, 218, ScaleConverter::get<LinearBipolar50PercentScaleConverter>(), 0, 50, 500));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 219, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(
      new Parameter(this, 221, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(), 0.5, 100, 1000));

  appendParameter(new Parameter(this, 222, ScaleConverter::get<Linear60To140StScaleConverter>(), 0.75, 80, 800));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 223, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 307, ScaleConverter::get<Linear100PercentScaleConverter>(), 0.0, 100, 1000));

  appendParameter(new ModulateableParameter(this, 308, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
                                            0.0, 100, 1000));

  appendParameter(
      new ModulateableParameter(this, 310, ScaleConverter::get<Linear0To140StScaleConverter>(), 1.0, 140, 1400));
}
