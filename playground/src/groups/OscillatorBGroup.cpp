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

OscillatorBGroup::OscillatorBGroup(ParameterGroupSet *parent)
    : ParameterGroup(parent, "Osc B", "Oscillator B", "Oscillator B", "Oscillator B")
{
}

OscillatorBGroup::~OscillatorBGroup()
{
}

void OscillatorBGroup::init()
{
  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 83, ScaleConverter::get<PitchOscLinearStScaleConverter>(),
      ScaleConverter::get<PitchOscLinearStScaleModulationConverter>(), 80.0 / 150.0, 150, 15000));

  appendParameter(
      new Parameter(this, 85, ScaleConverter::get<Fine105PercentScaleConverter>(), 100.0 / 105.0, 105, 10500));

  appendParameter(new Parameter(this, 86, ScaleConverter::get<FineBipolar80StScaleConverter>(), 0, 80, 8000));

  appendParameter(
      new ModulateableParameter(this, 87, ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 89, ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 90, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 92, ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 93, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 94, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 96, ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 97, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 98, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 100, ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 302, ScaleConverter::get<PhaseBipolar180DegreeScaleConverter>(), 0, 180, 1800));

  appendParameter(new Parameter(this, 304, ScaleConverter::get<Linear80To140StScaleConverter>(), 4.0 / 6.0, 60, 600));
}
