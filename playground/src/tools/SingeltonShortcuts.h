#pragma once

#include <proxies/hwui/HWUIEnums.h>

class Parameter;

namespace SiSc
{
  LayoutVersionMode getLayoutSetting();
  namespace EB {
    const Parameter* getCurrentParameter();
  }
};