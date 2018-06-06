#pragma once

#include "TemplateEnums.h"
#include "ControlClass.h"

namespace DescriptiveLayouts
{
  class GenericControl;

  struct ControlInstance
  {
    public:
      struct EventConnection
      {
          EventSources src;
          PrimitiveInstances targetInstance;
          PrimitiveProperty targetProperty;
      };

      using EventConnections = std::list<EventConnection>;

      ControlInstance(ControlInstances controlInstance, ControlClasses control, Point position, const EventConnections &eventConnections);

      Control* instantiate() const;

    private:
      ControlInstances controlInstance;
      ControlClasses controlClass;
      Point position;
      EventConnections eventConnections;

      friend class GenericControl;
  };
}
