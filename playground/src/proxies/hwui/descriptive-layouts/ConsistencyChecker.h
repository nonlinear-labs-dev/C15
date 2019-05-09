#pragma once

#include <ostream>

namespace DescriptiveLayouts
{

  class ConsistencyChecker
  {
   public:
    ConsistencyChecker(std::ostream &out);

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
