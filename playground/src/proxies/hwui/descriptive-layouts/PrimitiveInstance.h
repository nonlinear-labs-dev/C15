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

      PrimitiveInstance(PrimitiveInstances primitiveInstance, PrimitiveClasses type, Rect relativePosition, PrimitiveTag t =
          PrimitiveTag::Any, DefaultText defaultText = "", Converter converter = nullptr);

      Control* instantiate() const;

      PrimitiveInstances primitiveInstance;
      PrimitiveClasses primitveClass;
      Rect relativePosition;
      DefaultText defaultText;
      PrimitiveTag tag;
      std::function<std::any(std::any)> converter;
  };

}
