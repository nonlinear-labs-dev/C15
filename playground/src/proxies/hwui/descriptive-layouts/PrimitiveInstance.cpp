#include "PrimitiveInstance.h"
#include "PrimitivesFactory.h"
#include "Styles.h"

namespace DescriptiveLayouts
{
  PrimitiveInstance::PrimitiveInstance(PrimitiveInstances primitiveInstance, PrimitiveClasses type, Rect relativePosition, PrimitiveTag t,
                                       DefaultText defaultText, Converter converter) :
      primitiveInstance(primitiveInstance),
      tag(t),
      primitveClass(type),
      relativePosition(relativePosition),
      defaultText(defaultText),
      converter(converter)
  {
  }

  Control* PrimitiveInstance::instantiate() const
  {
    return PrimitivesFactory::create(*this);
  }
}
