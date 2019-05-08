#pragma once

class Control;

namespace DescriptiveLayouts
{

  struct PrimitiveInstance;

  struct PrimitivesFactory
  {
    static Control* create(const PrimitiveInstance& c);
  };
}
