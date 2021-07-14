#include <utility>

#pragma once

#include "TemplateEnums.h"
#include "ControlClass.h"

namespace DescriptiveLayouts
{
  class GenericControl;
  class EventProvider;

  struct ControlInstance
  {
   public:
    struct EventConnection
    {
      EventSources src;
      PrimitiveInstances targetInstance;
      PrimitiveProperty targetProperty;
    };

    struct StaticInitList
    {

      using tPrimitiveInstance = Glib::ustring;
      using tPrimitiveProperty = PrimitiveProperty;
      using tValue = std::experimental::any;

      struct InitEntry
      {
        tPrimitiveInstance m_instance;
        tPrimitiveProperty m_property;
        tValue m_value;

        InitEntry(tPrimitiveInstance i, tPrimitiveProperty p, tValue v)
            : m_instance { std::move(i) }
            , m_property { p }
            , m_value { std::move(v) } {};
      };

      void addToList(tPrimitiveInstance i, tPrimitiveProperty p, tValue v)
      {
        m_inits.emplace_back(InitEntry(std::move(i), p, std::move(v)));
      };

      std::vector<InitEntry> m_inits;
    };

    using EventConnections = std::list<EventConnection>;

    ControlInstance(ControlInstances controlInstance, ControlClasses control, const Point& position,
                    EventConnections eventConnections, StaticInitList staticInit);

    Control* instantiate(EventProvider* eventProvider) const;

    ControlInstances controlInstance;
    ControlClasses controlClass;
    Point position;
    EventConnections eventConnections;
    StaticInitList staticInitList;

    friend class GenericControl;
    friend class ConsistencyChecker;
  };
}
