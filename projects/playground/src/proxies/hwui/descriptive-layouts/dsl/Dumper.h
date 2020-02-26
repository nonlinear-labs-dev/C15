#pragma once

#include <stdlib.h>

namespace DescriptiveLayouts
{
  class Element;

  class Dumper
  {
   public:
    static void dump(const Element &e, size_t indent = 0);
  };
}
