#include <utility>

#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MacroControlEditButtonMenu.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationCarousel.h>
#include "ControlInstance.h"
#include "GenericControl.h"
#include "proxies/hwui/panel-unit/boled/parameter-screens/controls/ParameterEditButtonMenu.h"

namespace DescriptiveLayouts
{
  ControlInstance::ControlInstance(ControlInstances controlInstance, ControlClasses control, Point position,
                                   const EventConnections& eventConnections)
      : controlInstance(std::move(controlInstance))
      , controlClass(std::move(control))
      , position(position)
      , eventConnections(eventConnections)
  {
  }

  Control* ControlInstance::instantiate() const
  {
    if(controlClass == "MacroControlEditButtonMenu")
    {
      return new MacroControlEditButtonMenu(Rect(position.getX(), position.getY(), 58, 62));
    }
    if(controlClass == "ModulationCarousel")
    {
      return new ModulationCarousel(ModulationCarousel::Mode::None, Rect(position.getX(), position.getY(), 58, 62));
    }
    if(controlClass == "ParameterEditButtonMenu")
    {
      return new ParameterEditButtonMenu(Rect(position.getX(), position.getY(), 58, 62));
    }
    return new GenericControl(*this);
  }
}
