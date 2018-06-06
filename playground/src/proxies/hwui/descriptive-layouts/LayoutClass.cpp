#include "LayoutClass.h"
#include "ControlInstance.h"
#include "GenericLayout.h"

namespace DescriptiveLayouts
{

  void LayoutClass::addToList(EventSinkMapping s)
  {
    sinkMappings.push_back(s);
  }

  void LayoutClass::addToList(ControlInstance s)
  {
    controls.push_back(s);
  }

  void LayoutClass::addToList(Selector s)
  {
    selectors.push_back(s);
  }

  void LayoutClass::addToList(std::function<bool()> c)
  {
    conditions.push_back(c);
  }

  DFBLayout* LayoutClass::instantiate() const
  {
    DebugLevel::warning("instantiating Layout:", id);
    return new GenericLayout(*this);
  }

  bool LayoutClass::matches(FocusAndMode fam) const
  {
    return std::all_of(selectors.begin(), selectors.end(), [=](const Selector& s)
    {
      return s.test(fam);
    });
  }

  bool LayoutClass::meetsConditions() const
  {
    return conditions.size() == 0 || std::all_of(conditions.begin(), conditions.end(), [](const std::function<bool()> c) {
      return c();
    });
  }

  const unsigned long LayoutClass::getWeight() const {
    return conditions.size() + selectors.size();
  }
}
