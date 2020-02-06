#include "OscillatorAGroup.h"

#include "../parameters/scale-converters/IdentityScaleConverter.h"

#include "../parameters/scale-converters/FineBipolar80StScaleConverter.h"

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
#include <parameters/scale-converters/LinearBipolar200PercentScaleConverter.h>
#include <parameters/scale-converters/PitchOscLinearStScaleModulationConverter.h>
#include <parameters/ModulateableParameterWithUnusualModUnit.h>
#include <parameters/scale-converters/OnOffScaleConverter.h>

OscillatorAGroup::OscillatorAGroup(ParameterDualGroupSet *parent, VoiceGroup vg)
    : ParameterGroup(parent, { "Osc A", vg }, "Oscillator A", "Oscillator A", "Oscillator A")
{
}

OscillatorAGroup::~OscillatorAGroup()
{
}

void OscillatorAGroup::init()
{
  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, { 53, getVoiceGroup() }, ScaleConverter::get<PitchOscLinearStScaleConverter>(),
      ScaleConverter::get<PitchOscLinearStScaleModulationConverter>(), 80.0 / 150.0, 150, 15000));

  appendParameter(new Parameter(this, { 55, getVoiceGroup() }, ScaleConverter::get<Fine105PercentScaleConverter>(),
                                100.0 / 105.0, 105, 10500));

  appendParameter(
      new Parameter(this, { 56, getVoiceGroup() }, ScaleConverter::get<FineBipolar80StScaleConverter>(), 0, 80, 8000));

  appendParameter(new ModulateableParameter(this, { 57, getVoiceGroup() },
                                            ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, { 59, getVoiceGroup() }, ScaleConverter::get<Linear100PercentScaleConverter>(), 0,
                                100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, { 60, getVoiceGroup() }, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, { 62, getVoiceGroup() }, ScaleConverter::get<Linear100PercentScaleConverter>(), 0,
                                100, 1000));

  appendParameter(new Parameter(this, { 63, getVoiceGroup() },
                                ScaleConverter::get<LinearBipolar100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, { 64, getVoiceGroup() }, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, { 66, getVoiceGroup() }, ScaleConverter::get<Linear100PercentScaleConverter>(), 0,
                                100, 1000));

  appendParameter(new Parameter(this, { 67, getVoiceGroup() },
                                ScaleConverter::get<LinearBipolar100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, { 68, getVoiceGroup() }, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, { 70, getVoiceGroup() }, ScaleConverter::get<Linear100PercentScaleConverter>(), 0,
                                100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(this, { 301, getVoiceGroup() },
                                            ScaleConverter::get<PhaseBipolar180DegreeScaleConverter>(), ScaleConverter::get<PhaseBipolar360DegreeScaleConverter>(), 0, 180, 1800));

  appendParameter(new Parameter(this, { 303, getVoiceGroup() }, ScaleConverter::get<Linear80To140StScaleConverter>(),
                                4.0 / 6.0, 60, 600));

  appendParameter(new Parameter(this, { 393, getVoiceGroup() }, ScaleConverter::get<OnOffScaleConverter>(), 1, 1, 1));
}
