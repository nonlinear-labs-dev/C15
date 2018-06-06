#pragma once

#include <proxies/hwui/descriptive-layouts/LayoutFactory.h>
#include <any>
#include "TemplateEnums.h"

namespace DescriptiveLayouts
{

  class PropertyOwner
  {
    public:
      virtual void setProperty(PrimitiveProperty key, std::any value) = 0;
  };

}
