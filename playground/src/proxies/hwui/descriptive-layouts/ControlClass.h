#pragma once

#include "TemplateEnums.h"
#include "PrimitiveInstance.h"
#include <list>
#include <utility>

namespace DescriptiveLayouts
{
  class ControlClass
  {
   public:
    ControlClass(ControlClasses controlClass, std::list<PrimitiveInstance> primitives)
        : controlClass(std::move(controlClass))
        , primitves(std::move(primitives))
    {
    }

    ControlClass(ControlClass &&o) = default;

    ControlClasses controlClass;
    std::list<PrimitiveInstance> primitves;
  };
}
