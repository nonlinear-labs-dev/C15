#include "ShaperAGroup.h"
#include "parameters/Parameter.h"
#include "parameters/ModulateableParameter.h"

#include "parameters/scale-converters/Linear100PercentScaleConverter.h"
#include "parameters/scale-converters/LinearBipolar100PercentScaleConverter.h"
#include "parameters/scale-converters/Linear50DbScaleConverter.h"
#include "parameters/scale-converters/LinearBipolar50DbScaleConverter.h"
#include <parameters/scale-converters/LinearBipolar200PercentScaleConverter.h>
#include <parameters/ModulateableParameterWithUnusualModUnit.h>

ShaperAGroup::ShaperAGroup(ParameterGroupSet *parent)
    : ParameterGroup(parent, "Sh A", "Shaper A", "Shaper A", "Shaper A")
{
}

ShaperAGroup::~ShaperAGroup()
{
}

void ShaperAGroup::init()
{
  appendParameter(new ModulateableParameterWithUnusualModUnit(this, 71,

                                                              ScaleConverter::get<Linear50DbScaleConverter>(),
                                                              ScaleConverter::get<LinearBipolar50DbScaleConverter>(),
                                                              0.2, 100, 500));

  appendParameter(new Parameter(this, 73,

                                ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 74,

                                ScaleConverter::get<Linear100PercentScaleConverter>(), 0.5, 100, 1000));

  appendParameter(new Parameter(this, 75,

                                ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, 76,

      ScaleConverter::get<LinearBipolar100PercentScaleConverter>(),
      ScaleConverter::get<LinearBipolar200PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameter(this, 78,

                                            ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, 80,

                                ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameter(this, 81,

                                            ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));
}
