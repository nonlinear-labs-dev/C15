#pragma once

#include <string>
#include "ControlClass.h"

namespace DescriptiveLayouts
{

  class ControlRegistry
  {
   protected:
    ControlRegistry();

   public:
    static ControlRegistry& get();

    void registerControl(ControlClass&& cp);
    const ControlClass& find(ControlClasses id) const;
    const bool exists(ControlClasses id) const;
    void clear();

   private:
    std::map<ControlClasses, ControlClass> m_controlRegistry;
    std::vector<ControlClasses> m_discreteRegistry;

    friend class ConsistencyChecker;
  };
}
