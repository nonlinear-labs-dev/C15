#pragma once

#include "Element.h"

namespace DescriptiveLayouts
{
  class Parser
  {
   public:
    Parser();

    static Element parseString(const std::string &s);
  };
}
