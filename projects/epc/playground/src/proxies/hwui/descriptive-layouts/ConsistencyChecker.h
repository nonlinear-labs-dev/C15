#pragma once

#include <ostream>
#include "LayoutClass.h"

namespace DescriptiveLayouts
{

  class ConsistencyChecker
  {
   public:
    explicit ConsistencyChecker(std::ostream &out);

    bool checkAll();

   private:
    bool checkLayoutSelectorsUnique();
    bool checkLayoutsContainOnlyKnownControlClasses();
    bool checkEventTargetsAreKnown();
    bool checkStyleSelectorsUseKnownEntities();
    bool checkAllControlClassesAreUsed();

    std::ostream &m_out;
  };
}
