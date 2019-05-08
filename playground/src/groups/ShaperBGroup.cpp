#include "ShaperBGroup.h"
#include "parameters/Parameter.h"
#include "parameters/ModulateableParameter.h"

#include "parameters/scale-converters/Linear100PercentScaleConverter.h"
#include "parameters/scale-converters/LinearBipolar100PercentScaleConverter.h"
#include "parameters/scale-converters/Linear50DbScaleConverter.h"
#include "parameters/scale-converters/LinearBipolar50DbScaleConverter.h"
#include <parameters/scale-converters/LinearBipolar200PercentScaleConverter.h>
#include <parameters/ModulateableParameterWithUnusualModUnit.h>

ShaperBGroup::ShaperBGroup(ParameterGroupSet *parent)
    : ParameterGroup(parent, "Sh B", "Shaper B", "Shaper B", "Shaper B")
{
}

ShaperBGroup::~ShaperBGroup()
{
}

void ShaperBGroup::init()
{
  appendParameter(new ModulateableParameterWithUnusualModUnit(this, 101,

                                                              ScaleConverter::get<Linear50DbScaleConverter>(),
                                                              ScaleConverter::get<LinearBipolar50DbScaleConverter>(),
                                                              0.2, 100, 500));

  appendParameter(new Parameter(this, 103,

                                ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 104,

                                ScaleConverter::get<Linear100PercentScaleConverter>(), 0.5, 100, 1000));

  appendParameter(new Parameter(this, 105,

                                ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 106,

      ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameter(this, 108,

                                            ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 110,

                                ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameter(this, 111,

                                            ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));
}
