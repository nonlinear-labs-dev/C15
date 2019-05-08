#include "ConsistencyChecker.h"
#include "LayoutFactory.h"
#include "ControlRegistry.h"
#include "Styles.h"

namespace DescriptiveLayouts
{
  ConsistencyChecker::ConsistencyChecker(std::ostream &out)
      : m_out(out)
  {
  }

  bool ConsistencyChecker::checkAll()
  {
    bool ret = checkLayoutSelectorsUnique();
    ret &= checkLayoutsContainOnlyKnownControlClasses();
    ret &= checkEventTargetsAreKnown();
    ret &= checkStyleSelectorsUseKnownEntities();
    ret &= checkAllControlClassesAreUsed();
    return ret;
  }

  bool ConsistencyChecker::checkLayoutSelectorsUnique()
  {
    const auto &factory = BoledLayoutFactory::get();

    for(const auto &one : factory.m_layouts)
    {
      for(const auto &other : factory.m_layouts)
      {
        if(&one != &other)
        {
          if(one.selectors.size() == other.selectors.size())
          {
            bool foundDifference = false;

            for(const auto &oneSelector : one.selectors)
            {
              for(const auto &otherSelector : other.selectors)
              {
                if(oneSelector == otherSelector)
                {
                  continue;
                }

                foundDifference = true;
              }
            }

            if(!foundDifference)
            {
              m_out << "Error: Two layouts have identical selectors: " << one.id << " and " << other.id << std::endl;
              return false;
            }
          }
        }
      }
    }

    return true;
  }

  bool ConsistencyChecker::checkLayoutsContainOnlyKnownControlClasses()
  {
    const auto &factory = BoledLayoutFactory::get();

    for(const auto &layout : factory.m_layouts)
    {
      for(const auto &control : layout.controls)
      {
        if(!ControlRegistry::get().exists(control.controlClass))
        {
          m_out << "Layout " << layout.id << " references unknown control class " << control.controlClass << std::endl;
          return false;
        }
      }
    }

    return true;
  }

  bool ConsistencyChecker::checkEventTargetsAreKnown()
  {
    const auto &factory = BoledLayoutFactory::get();

    for(const auto &layout : factory.m_layouts)
    {
      for(const auto &control : layout.controls)
      {
        for(const auto &c : control.eventConnections)
        {
          const auto &targetInstance = c.targetInstance;
          const auto &controlClass = ControlRegistry::get().find(control.controlClass);

          auto findResult = std::find_if(controlClass.primitves.begin(), controlClass.primitves.end(),
                                         [&](const auto &i) { return i.primitiveInstance == targetInstance; });

          if(findResult == controlClass.primitves.end())
          {
            m_out << "Event map for layout " << layout.id << " references unknown primitive instance " << targetInstance
                  << std::endl;
          }
        }
      }
    }

    return true;
  }

  bool ConsistencyChecker::checkStyleSelectorsUseKnownEntities()
  {
    const auto &styles = StyleSheet::get();
    const auto &layoutFactory = BoledLayoutFactory::get();

    bool result = true;

    styles.iterateStyles([&](const Detail::StyleSelector &selector, const auto &style) {
      bool foundControlInstance = selector.ci == ControlInstances::Any;
      bool foundLayoutClass = selector.l == LayoutClasses::Any;
      bool foundPrimitiveInstance = selector.pi == PrimitiveInstances::Any;
      bool foundPrimitiveTag = selector.pt == PrimitiveTag::Any;

      for(const auto &layout : layoutFactory.m_layouts)
      {
        foundLayoutClass |= layout.id == selector.l;

        for(const auto &control : layout.controls)
        {
          foundControlInstance |= control.controlInstance == selector.ci;

          try
          {
            const auto &ctrl = ControlRegistry::get().find(selector.cc);

            for(const auto p : ctrl.primitves)
            {
              foundPrimitiveInstance |= p.primitiveInstance == selector.pi;
              foundPrimitiveTag |= p.tag == selector.pt;
            }
          }
          catch(...)
          {
            m_out << "Style " << style.name << " references unknown control class " << selector.cc << std::endl;
            result = false;
          }
        }
      }

      if(!foundLayoutClass)
      {
        m_out << "Style " << style.name << " contains selector for not existing layout " << selector.l << std::endl;
        result = false;
      }

      if(!foundControlInstance)
      {
        m_out << "Style " << style.name << " references unknown control instance " << selector.ci << std::endl;
        result = false;
      }

      if(!foundPrimitiveInstance)
      {
        m_out << "Style " << style.name << " references unknown primitive instance " << selector.pi << std::endl;
        result = false;
      }

      if(!foundPrimitiveTag)
      {
        m_out << "Style " << style.name << " references unknown primitive tag " << selector.pt << std::endl;
        result = false;
      }
    });

    return result;
  }

  bool ConsistencyChecker::checkAllControlClassesAreUsed()
  {
    const auto &layoutFactory = BoledLayoutFactory::get();

    for(const auto &a : ControlRegistry::get().m_controlRegistry)
    {
      bool found = false;

      for(const auto &layout : layoutFactory.m_layouts)
      {
        for(const auto &control : layout.controls)
        {
          found |= control.controlClass == a.first;
          if(found)
            break;
        }
      }
      if(!found)
      {
        m_out << "Control class " << a.first << " is never used in a layout." << std::endl;
        return false;
      }
    }
    return true;
  }
}
