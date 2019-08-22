#include "GenericControl.h"
#include "GenericLayout.h"
#include <tools/EnumTools.h>
#include "Styleable.h"
#include "proxies/hwui/descriptive-layouts/EventSources/EventSource.h"
#include "PropertyOwner.h"
#include "ControlRegistry.h"

namespace DescriptiveLayouts
{
  GenericControl::GenericControl(const ControlInstance &prototype)
      : ControlWithChildren(Rect(prototype.position, Point(0, 0)))
      , m_prototype(prototype)
  {
    addPrimitives();
  }

  GenericControl::~GenericControl()
  {
    for(auto &c : m_connections)
      c.disconnect();
  }

  void GenericControl::addPrimitives()
  {
    auto rect = getPosition();

    int maxX = 0;
    int maxY = 0;

    const ControlClass &controlClass = ControlRegistry::get().find(m_prototype.controlClass);

    for(auto &p : controlClass.primitves)
    {
      auto c = addControl(p.instantiate());

      for(auto &init : m_prototype.staticInitList.m_inits)
      {
        if(p.primitiveInstance == init.m_instance || init.m_instance == "Any")
        {
          if(auto propertyOwner = dynamic_cast<PropertyOwner *>(c))
          {
            propertyOwner->setProperty(init.m_property, init.m_value);
          }
        }
      }

      maxX = std::max(maxX, c->getPosition().getRight());
      maxY = std::max(maxY, c->getPosition().getBottom());
    }

    rect.setWidth(maxX + 1);
    rect.setHeight(maxY + 1);
    setPosition(rect);
  }

  void GenericControl::style(const LayoutClasses &layout)
  {
    if(m_lastUsedLayout != layout)
      m_lastUsedLayout = layout;

    DebugLevel::info("Styling control", m_prototype.controlInstance, "of class", m_prototype.controlClass);

    for(auto &p : getControls())
    {
      if(auto a = std::dynamic_pointer_cast<Styleable>(p))
      {
        a->style(layout, m_prototype.controlClass, m_prototype.controlInstance);
      }
    }
  }

  void GenericControl::connect()
  {
    for(auto &c : m_prototype.eventConnections)
    {
      m_connections.push_back(EventSourceBroker::get().connect(
          c.src, sigc::bind<1>(sigc::mem_fun(this, &GenericControl::onEventFired), c)));
    }

    for(auto &c : m_prototype.visibility.m_items)
    {
      m_connections.push_back(EventSourceBroker::get().connect(
          c.m_source, sigc::bind(sigc::mem_fun(this, &GenericControl::onVisibilityChanged), m_prototype, c)));

      m_controlVisible[c.m_source] = std::experimental::any_cast<bool>(EventSourceBroker::get().evaluate(c.m_source));
    }
  }

  void GenericControl::onEventFired(const std::experimental::any &v, const ControlInstance::EventConnection &connection)
  {
    bool shouldRestyle = false;
    for(const auto &c : getControls())
    {
      if(auto a = std::dynamic_pointer_cast<Styleable>(c))
      {
        const auto &primitive = a->getPrimitive();

        if(primitive.primitiveInstance == connection.targetInstance || connection.targetInstance == "Any")
        {
          if(auto p = std::dynamic_pointer_cast<PropertyOwner>(c))
          {
            p->setProperty(connection.targetProperty, v);
            shouldRestyle = true;
          }
        }
      }
    }
    if(shouldRestyle)
    {
      setDirty();
      style(m_lastUsedLayout);
    }
  }

  void GenericControl::onVisibilityChanged(std::experimental::any visibility, const ControlInstance &instance,
                                           const ControlInstance::VisibilityItem &item)
  {
    if(m_prototype.controlInstance == instance.controlInstance)
    {
      try
      {
        auto visible = std::experimental::any_cast<bool>(visibility);

        if(item.inverted)
          visible = !visible;

        m_controlVisible[item.m_source] = visible;

        setVisible(std::all_of(m_controlVisible.begin(), m_controlVisible.end(), [](auto e) { return e.second; }));
      }
      catch(...)
      {
        DebugLevel::warning("Could not connect:", toString(item.m_source),
                            "to visibility of:", instance.controlInstance, "! event does not evaluate to boolean!");
      }
    }
  }
}
