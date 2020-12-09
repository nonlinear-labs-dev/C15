#include <utility>
#include "ControlInstance.h"
#include "GenericControl.h"
#include "ControlRegistry.h"

namespace DescriptiveLayouts
{
  ControlInstance::ControlInstance(ControlInstances controlInstance, ControlClasses control, const Point& position,
                                   EventConnections eventConnections, StaticInitList staticInit)
      : controlInstance(std::move(controlInstance))
      , controlClass(std::move(control))
      , position(position)
      , eventConnections(std::move(eventConnections))
      , staticInitList(std::move(staticInit))
  {
  }

  Control* ControlInstance::instantiate(EventProvider* eventProvider) const
  {
    if(ControlRegistry::get().exists(controlClass))
      if(auto control = ControlRegistry::get().instantiateConcrete(controlClass, position))
        return control;

    return new GenericControl(*this, eventProvider);
  }
}
