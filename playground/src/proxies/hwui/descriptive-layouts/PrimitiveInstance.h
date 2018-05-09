#pragma once

#include <functional>
#include <any>
#include "TemplateEnums.h"
#include <proxies/hwui/controls/Rect.h>

class Control;

namespace DescriptiveLayouts
{

  class PrimitiveInstance
  {
    public:
      using Converter = std::function<std::any(std::any)>;

      PrimitiveInstance(PrimitiveInstances primitiveInstance, PrimitiveClasses type, Rect relativePosition, PrimitiveTag t = PrimitiveTag::Any, PrimitiveProperty eventTarget = PrimitiveProperty::None,
                Converter converter = nullptr);

      Control* instantiate() const;

      PrimitiveInstances primitiveInstance;
      PrimitiveClasses primitveClass;
      Rect relativePosition;
      PrimitiveProperty eventTarget;
      PrimitiveTag tag;
      std::function<std::any(std::any)> converter;
  };

}
