#pragma once

#include <proxies/hwui/descriptive-layouts/LayoutFactory.h>
#include <experimental/any>
#include "TemplateEnums.h"


namespace DescriptiveLayouts
{
  class PropertyOwner
  {
   public:
    virtual void setProperty(PrimitiveProperty key, std::experimental::any value) = 0;
  };
}
