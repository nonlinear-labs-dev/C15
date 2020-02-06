#include "OscillatorBGroup.h"

#include "parameters/Parameter.h"
#include "parameters/ModulateableParameter.h"

#include "parameters/scale-converters/IdentityScaleConverter.h"
#include "parameters/scale-converters/PitchOscLinearStScaleConverter.h"
#include "parameters/scale-converters/LinearBipolar150StScaleConverter.h"
#include "parameters/scale-converters/PhaseBipolar180DegreeScaleConverter.h"
#include "parameters/scale-converters/PhaseBipolar360DegreeScaleConverter.h"
#include "parameters/scale-converters/LinearBipolar100PercentScaleConverter.h"
#include "parameters/scale-converters/Linear100PercentScaleConverter.h"
#include "parameters/scale-converters/Fine105PercentScaleConverter.h"
#include "parameters/scale-converters/Linear80To140StScaleConverter.h"
#include "parameters/scale-converters/IdentityScaleConverter.h"
#include "parameters/scale-converters/FineBipolar80StScaleConverter.h"
#include <parameters/scale-converters/LinearBipolar200PercentScaleConverter.h>
#include <parameters/scale-converters/PitchOscLinearStScaleModulationConverter.h>
#include <parameters/ModulateableParameterWithUnusualModUnit.h>
#include <parameters/scale-converters/OnOffScaleConverter.h>

OscillatorBGroup::OscillatorBGroup(ParameterDualGroupSet *parent, VoiceGroup vg)
    : ParameterGroup(parent, { "Osc B", vg }, "Oscillator B", "Oscillator B", "Oscillator B")
{
}

OscillatorBGroup::~OscillatorBGroup()
{
}

void OscillatorBGroup::init()
{
  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, { 83, getVoiceGroup() }, ScaleConverter::get<PitchOscLinearStScaleConverter>(),
      ScaleConverter::get<PitchOscLinearStScaleModulationConverter>(), 80.0 / 150.0, 150, 15000));

  appendParameter(new Parameter(this, { 85, getVoiceGroup() }, ScaleConverter::get<Fine105PercentScaleConverter>(),
                                100.0 / 105.0, 105, 10500));

  appendParameter(
      new Parameter(this, { 86, getVoiceGroup() }, ScaleConverter::get<FineBipolar80StScaleConverter>(), 0, 80, 8000));

  appendParameter(new ModulateableParameter(this, { 87, getVoiceGroup() },
                                            ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, { 89, getVoiceGroup() }, ScaleConverter::get<Linear100PercentScaleConverter>(), 0,
                                100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, { 90, getVoiceGroup() }, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, { 92, getVoiceGroup() }, ScaleConverter::get<Linear100PercentScaleConverter>(), 0,
                                100, 1000));

  appendParameter(new Parameter(this, { 93, getVoiceGroup() },
                                ScaleConverter::get<LinearBipolar100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, { 94, getVoiceGroup() }, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, { 96, getVoiceGroup() }, ScaleConverter::get<Linear100PercentScaleConverter>(), 0,
                                100, 1000));

  appendParameter(new Parameter(this, { 97, getVoiceGroup() },
                                ScaleConverter::get<LinearBipolar100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, { 98, getVoiceGroup() }, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, { 100, getVoiceGroup() }, ScaleConverter::get<Linear100PercentScaleConverter>(),
                                0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, { 302, getVoiceGroup() }, ScaleConverter::get<PhaseBipolar180DegreeScaleConverter>(),
      ScaleConverter::get<PhaseBipolar180DegreeScaleConverter>(), 0, 180, 1800));

  appendParameter(new Parameter(this, { 304, getVoiceGroup() }, ScaleConverter::get<Linear80To140StScaleConverter>(),
                                4.0 / 6.0, 60, 600));

  appendParameter(new Parameter(this, { 394, getVoiceGroup() }, ScaleConverter::get<OnOffScaleConverter>(), 1, 1, 1));
}
