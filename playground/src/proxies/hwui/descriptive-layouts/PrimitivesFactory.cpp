#include <tools/ExceptionTools.h>
#include <proxies/hwui/descriptive-layouts/Primitives/Circle.h>
#include "PrimitivesFactory.h"
#include "TemplateEnums.h"
#include "PrimitiveInstance.h"
#include "proxies/hwui/descriptive-layouts/Primitives/Bar.h"
#include "proxies/hwui/descriptive-layouts/Primitives/Border.h"
#include "proxies/hwui/descriptive-layouts/Primitives/Text.h"

namespace DescriptiveLayouts
{
  Control* PrimitivesFactory::create(const PrimitiveInstance& primitive)
  {
    switch(primitive.primitveClass)
    {
      case PrimitiveClasses::Bar:
        return new Bar(primitive);
      case PrimitiveClasses::Border:
        return new Border(primitive);
      case PrimitiveClasses::Text:
        return new Text(primitive);
      case PrimitiveClasses::Circle:
        return new Circle(primitive);
      default:
        throw ExceptionTools::TemplateException(Glib::ustring("PrimitiveInstance Type not found: ")
                                                    + toString(primitive.primitveClass),
                                                "__LINE__ __FILE__");
    }
  }
}
