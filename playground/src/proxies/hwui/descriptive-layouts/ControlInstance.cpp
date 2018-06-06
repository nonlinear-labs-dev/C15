#include "ControlInstance.h"
#include "GenericControl.h"
#include "proxies/hwui/panel-unit/boled/parameter-screens/controls/ParameterEditButtonMenu.h"

namespace DescriptiveLayouts
{
  ControlInstance::ControlInstance(ControlInstances controlInstance, ControlClasses control, Point position, const EventConnections &eventConnections) :
      controlInstance(controlInstance),
      controlClass(control),
      position(position),
      eventConnections(eventConnections)
  {
  }

  Control* ControlInstance::instantiate() const
  {
    if(controlClass == "ParameterEditButtonMenu")
    {
      return new ParameterEditButtonMenu(Rect(position.getX(), position.getY(), 58, 62));
    }
    return new GenericControl(*this);
  }
}
