#pragma once

#include "Element.h"

namespace DescriptiveLayouts
{
  bool resolveReferences(Element &e);
  Element flatten(const Element &tree);
}
