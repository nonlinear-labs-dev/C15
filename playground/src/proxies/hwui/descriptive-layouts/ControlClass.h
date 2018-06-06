#pragma once

#include "TemplateEnums.h"
#include "PrimitiveInstance.h"
#include <list>

namespace DescriptiveLayouts
{

  class ControlClass
  {
    public:
        ControlClass(ControlClasses controlClass, std::list<PrimitiveInstance> primitives) :
            controlClass(controlClass),
            primitves(primitives)
        {
        }

        ControlClass(ControlClass &&o) = default;

      ControlClasses controlClass;
      std::list<PrimitiveInstance> primitves;
  };

}
