#pragma once
#include <parameters/scale-converters/dimension/LegatoDimension.h>
#include "LinearCountScaleConverter.h"

class LegatoScaleConverter : public LinearCountScaleConverter<4, LegatoDimension>
{
};
