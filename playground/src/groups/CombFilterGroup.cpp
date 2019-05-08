#include "CombFilterGroup.h"

#include "parameters/ModulateableParameter.h"
#include "parameters/Parameter.h"

#include "parameters/scale-converters/Fine105PercentScaleConverter.h"
#include "parameters/scale-converters/Linear100PercentScaleConverter.h"
#include "parameters/scale-converters/PitchCombLinearStScaleConverter.h"
#include "parameters/scale-converters/Linear0To140StScaleConverter.h"
#include "parameters/scale-converters/LinearBipolar60StScaleConverter.h"
#include "parameters/scale-converters/LinearBipolar120StScaleConverter.h"
#include "parameters/scale-converters/LinearBipolar140StScaleConverter.h"
#include "parameters/scale-converters/Linear40To140StScaleConverter.h"
#include "parameters/scale-converters/LinearBipolar100PercentScaleConverter.h"
#include "parameters/scale-converters/FineBipolar80StScaleConverter.h"
#include "parameters/scale-converters/CombDecayBipolarMsScaleConverter.h"
#include "parameters/scale-converters/LinearBipolar100StScaleConverter.h"
#include <parameters/scale-converters/LinearBipolar200PercentScaleConverter.h>
#include <parameters/scale-converters/PitchCombLinearStModulationScaleConverter.h>
#include <parameters/ModulateableParameterWithUnusualModUnit.h>

CombFilterGroup::CombFilterGroup(ParameterGroupSet *parent)
    : ParameterGroup(parent, "Comb", "Comb Filter", "Comb Filter", "Comb Filter")
{
}

CombFilterGroup::~CombFilterGroup()
{
}

void CombFilterGroup::init()
{
  appendParameter(
      new ModulateableParameter(this, 113, ScaleConverter::get<Linear100PercentScaleConverter>(), 0.0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 115, ScaleConverter::get<PitchCombLinearStScaleConverter>(),
      ScaleConverter::get<PitchCombLinearStModulationScaleConverter>(), 0.5, 120, 12000));

  appendParameter(
      new Parameter(this, 117, ScaleConverter::get<Fine105PercentScaleConverter>(), 100.0 / 105.0, 105, 10500));

  appendParameter(new Parameter(this, 118, ScaleConverter::get<FineBipolar80StScaleConverter>(), 0, 80, 8000));

  appendParameter(
      new ModulateableParameter(this, 119, ScaleConverter::get<CombDecayBipolarMsScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 121, ScaleConverter::get<Linear100PercentScaleConverter>(), 0.33, 100, 1000));

  appendParameter(new Parameter(this, 122, ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(
      new ModulateableParameter(this, 305, ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 123, ScaleConverter::get<Linear0To140StScaleConverter>(),
      ScaleConverter::get<LinearBipolar140StScaleConverter>(), 1.0, 140, 1400));

  appendParameter(new Parameter(this, 125, ScaleConverter::get<Linear100PercentScaleConverter>(), 1.0, 100, 1000));

  appendParameter(new Parameter(this, 126, ScaleConverter::get<FineBipolar80StScaleConverter>(), 0, 80, 800));

  appendParameter(
      new ModulateableParameter(this, 127, ScaleConverter::get<Linear100PercentScaleConverter>(), 0.5, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 129, ScaleConverter::get<Linear40To140StScaleConverter>(),
      ScaleConverter::get<LinearBipolar100StScaleConverter>(), 1.0, 100, 1000));

  appendParameter(new Parameter(this, 131, ScaleConverter::get<Linear100PercentScaleConverter>(), 1.0, 100, 1000));

  appendParameter(new Parameter(this, 132, ScaleConverter::get<LinearBipolar100StScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 133, ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 135, ScaleConverter::get<Linear100PercentScaleConverter>(), 0.0, 100, 1000));
}
