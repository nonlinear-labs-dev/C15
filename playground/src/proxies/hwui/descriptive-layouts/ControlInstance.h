#include <utility>

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

    typedef struct VisibilityItem
    {
      EventSources m_source = EventSources::BooleanTrue;
      bool inverted = false;
    } VisibilityItem;

    typedef struct VisibilityEvent
    {
      std::vector<VisibilityItem> m_items;
    } VisibilityEvent;

    struct StaticInitList
    {

      using tPrimitiveInstance = Glib::ustring;
      using tPrimitiveProperty = PrimitiveProperty;
      using tValue = std::any;

      struct InitEntry
      {
        tPrimitiveInstance m_instance;
        tPrimitiveProperty m_property;
        tValue m_value;

        InitEntry(tPrimitiveInstance i, tPrimitiveProperty p, tValue v)
            : m_instance{ std::move(i) }
            , m_property{ p }
            , m_value{ std::move(v) } {};
      };

      void addToList(tPrimitiveInstance i, tPrimitiveProperty p, tValue v)
      {
        m_inits.emplace_back(InitEntry(std::move(i), p, std::move(v)));
      };

      std::vector<InitEntry> m_inits;
    };

    using EventConnections = std::list<EventConnection>;

    ControlInstance(ControlInstances controlInstance, ControlClasses control, Point position,
                    const EventConnections& eventConnections, StaticInitList staticInit, VisibilityEvent visibility);

    Control* instantiate() const;

    ControlInstances controlInstance;
    ControlClasses controlClass;
    Point position;
    EventConnections eventConnections;
    StaticInitList staticInitList;
    VisibilityEvent visibility;

    friend class GenericControl;
    friend class ConsistencyChecker;
  };
}
