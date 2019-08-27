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

  void LayoutClass::addToList(ConditionBase* c)
  {
    conditions.push_back(c);
  }

  DFBLayout* LayoutClass::instantiate() const
  {
    DebugLevel::info("instantiating Layout:", id);
    return new GenericLayout(*this);
  }

  bool LayoutClass::matches(FocusAndMode fam) const
  {
    return std::all_of(selectors.begin(), selectors.end(), [=](const Selector& s) { return s.test(fam); });
  }

  FocusAndMode LayoutClass::getDesiredFocusAndMode() const
  {
    FocusAndMode fam;
    for(auto& c : selectors)
    {
      try
      {
        fam.focus = c.getFocus();
        continue;
      }
      catch(...)
      {
      }
      try
      {
        fam.mode = c.getMode();
        continue;
      }
      catch(...)
      {
      }
      try
      {
        fam.detail = c.getDetail();
        continue;
      }
      catch(...)
      {
      }
    }
    return fam;
  }

  bool LayoutClass::meetsConditions() const
  {
    return conditions.empty()
        || std::all_of(conditions.begin(), conditions.end(), [](ConditionBase* c) { return c->get(); });
  }

  const unsigned long LayoutClass::getWeight() const
  {
    return conditions.size() + selectors.size();
  }

  const std::string LayoutClass::getName() const
  {
    return id;
  }
}
