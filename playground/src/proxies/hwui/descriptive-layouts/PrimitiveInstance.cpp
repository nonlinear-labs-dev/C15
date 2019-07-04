#include <utility>
#include "PrimitiveInstance.h"
#include "PrimitivesFactory.h"
#include "Styles.h"

namespace DescriptiveLayouts
{
  PrimitiveInstance::PrimitiveInstance(PrimitiveInstances primitiveInstance, PrimitiveClasses type,
                                       Rect relativePosition, PrimitiveTag t, Converter converter)
      : primitiveInstance(std::move(primitiveInstance))
      , tag(std::move(t))
      , primitveClass(type)
      , relativePosition(relativePosition)
      , converter(std::move(converter))
  {
  }

  Control* PrimitiveInstance::instantiate() const
  {
    return PrimitivesFactory::create(*this);
  }
}
