#pragma once

#include <tools/EnumTools.h>

namespace DescriptiveLayouts
{
  ENUM(LayoutConditions, uint8_t, IsParameterModulateable);

  bool isConditionFulfilled(LayoutConditions c);
}
