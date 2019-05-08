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

OscillatorAGroup::OscillatorAGroup(ParameterGroupSet *parent)
    : ParameterGroup(parent, "Osc A", "Oscillator A", "Oscillator A", "Oscillator A")
{
}

OscillatorAGroup::~OscillatorAGroup()
{
}

void OscillatorAGroup::init()
{
  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 53, ScaleConverter::get<PitchOscLinearStScaleConverter>(),
      ScaleConverter::get<PitchOscLinearStScaleModulationConverter>(), 80.0 / 150.0, 150, 15000));

  appendParameter(
      new Parameter(this, 55, ScaleConverter::get<Fine105PercentScaleConverter>(), 100.0 / 105.0, 105, 10500));

  appendParameter(new Parameter(this, 56, ScaleConverter::get<FineBipolar80StScaleConverter>(), 0, 80, 8000));

  appendParameter(
      new ModulateableParameter(this, 57, ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 59, ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 60, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 62, ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 63, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 64, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 66, ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 67, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 68, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 70, ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 301, ScaleConverter::get<PhaseBipolar180DegreeScaleConverter>(), 0, 180, 1800));

  appendParameter(new Parameter(this, 303, ScaleConverter::get<Linear80To140StScaleConverter>(), 4.0 / 6.0, 60, 600));
}
