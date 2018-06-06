#include "ControlRegistry.h"

namespace DescriptiveLayouts
{

  void ControlRegistry::registerControl(ControlClass &&cp)
  {
    m_controlRegistry.insert(std::make_pair(cp.controlClass, std::move(cp)));
  }

  void ControlRegistry::clear()
  {
    m_controlRegistry.clear();
  }

  ControlRegistry& ControlRegistry::get()
  {
    static ControlRegistry cr;
    return cr;
  }

  const ControlClass& ControlRegistry::find(ControlClasses id) const
  {
    return m_controlRegistry.at(id);
  }

}
