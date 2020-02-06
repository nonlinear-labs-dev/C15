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
#include <parameters/scale-converters/LinearBipolar140StScaleConverter.h>

FlangerGroup::FlangerGroup(ParameterDualGroupSet *parent, VoiceGroup vg)
    : ParameterGroup(parent, { "Flang", vg }, "Flanger", "Flanger", "Flanger")
{
}

FlangerGroup::~FlangerGroup()
{
}

void FlangerGroup::init()
{
  appendParameter(new ModulateableParameter(
      this, { 211, getVoiceGroup() }, ScaleConverter::get<BipolarParabolic100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, { 213, getVoiceGroup() }, ScaleConverter::get<Linear180DegreeScaleConverter>(),
                                0.5, 180, 1800));

  appendParameter(new ModulateableParameter(this, { 214, getVoiceGroup() },
                                            ScaleConverter::get<Parabolic10HzScaleConverter>(), 0.317, 100, 1000));

  appendParameter(new ModulateableParameter(this, { 216, getVoiceGroup() },
                                            ScaleConverter::get<Parabolic50MsScaleConverter>(), 0.317, 125, 1250));

  appendParameter(new Parameter(this, { 218, getVoiceGroup() },
                                ScaleConverter::get<LinearBipolar50PercentScaleConverter>(), 0, 50, 500));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, { 219, getVoiceGroup() }, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, { 221, getVoiceGroup() },
                                ScaleConverter::get<LinearBipolar100PercentScaleConverter>(), 0.5, 100, 1000));

  appendParameter(new Parameter(this, { 222, getVoiceGroup() }, ScaleConverter::get<Linear60To140StScaleConverter>(),
                                0.75, 80, 800));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, { 223, getVoiceGroup() }, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, { 307, getVoiceGroup() }, ScaleConverter::get<Linear100PercentScaleConverter>(),
                                0.0, 100, 1000));

  appendParameter(new ModulateableParameter(
      this, { 308, getVoiceGroup() }, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(), 0.0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, { 310, getVoiceGroup() }, ScaleConverter::get<Linear0To140StScaleConverter>(),
      ScaleConverter::get<LinearBipolar140StScaleConverter>(), 1.0, 140, 1400));

  appendParameter(new ModulateableParameter(this, { 389, getVoiceGroup() },
                                            ScaleConverter::get<Linear100PercentScaleConverter>(), 0.0, 100, 1000));
}
