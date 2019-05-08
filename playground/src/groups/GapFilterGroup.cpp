#include "GapFilterGroup.h"
#include "parameters/Parameter.h"
#include "parameters/ModulateableParameter.h"

#include "parameters/scale-converters/Linear24To120StScaleConverter.h"
#include "parameters/scale-converters/LinearBipolar96StScaleConverter.h"
#include "parameters/scale-converters/LinearBipolar36StScaleConverter.h"
#include "parameters/scale-converters/Linear96StScaleConverter.h"
#include "parameters/scale-converters/LinearBipolar100PercentScaleConverter.h"
#include "parameters/scale-converters/Linear100PercentScaleConverter.h"
#include <parameters/scale-converters/LinearBipolar200PercentScaleConverter.h>
#include <parameters/ModulateableParameterWithUnusualModUnit.h>

GapFilterGroup::GapFilterGroup(ParameterGroupSet *parent)
    : ParameterGroup(parent, "Gap Filt", "Gap Filter", "Gap Filter", "Gap Filter")
{
}

GapFilterGroup::~GapFilterGroup()
{
}

void GapFilterGroup::init()
{
  appendParameter(new ModulateableParameterWithUnusualModUnit(this, 201,

                                                              ScaleConverter::get<Linear24To120StScaleConverter>(),
                                                              ScaleConverter::get<LinearBipolar96StScaleConverter>(),
                                                              0.5, 96, 960));

  appendParameter(new Parameter(this, 203,

                                ScaleConverter::get<LinearBipolar36StScaleConverter>(), 0, 72, 720));

  appendParameter(new ModulateableParameterWithUnusualModUnit(this, 204,

                                                              ScaleConverter::get<Linear96StScaleConverter>(),
                                                              ScaleConverter::get<LinearBipolar96StScaleConverter>(),
                                                              12.0 / 96.0, 96, 960));

  appendParameter(new Parameter(this, 206,

                                ScaleConverter::get<Linear100PercentScaleConverter>(), 0.5, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 207,

      ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 209,

      ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));
}
