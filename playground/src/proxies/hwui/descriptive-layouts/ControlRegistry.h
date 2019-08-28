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
    const ControlClass& find(const ControlClasses& id) const;
    bool exists(const ControlClasses& id) const;
    Control* instantiateConcrete(const ControlClasses& cc, const Point& position);
    void clear();

   private:
    typedef std::function<Control*(const Point&)> tFactoryCB;
    std::map<ControlClasses, ControlClass> m_controlRegistry;
    std::map<ControlClasses, tFactoryCB> m_discreteRegistry;

    friend class ConsistencyChecker;
  };
}
