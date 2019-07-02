#include "GenericControl.h"
#include "GenericLayout.h"
#include <tools/EnumTools.h>
#include "Styleable.h"
#include "EventSource.h"
#include "PropertyOwner.h"
#include "ControlRegistry.h"


namespace DescriptiveLayouts
{
  GenericControl::GenericControl(const ControlInstance &prototype, GenericLayout* pLayout)
      : ControlWithChildren(Rect(prototype.position, Point(0, 0)))
      , m_prototype(prototype)
      , m_genericLayout(pLayout)
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
      maxX = std::max(maxX, c->getPosition().getRight());
      maxY = std::max(maxY, c->getPosition().getBottom());
    }

    rect.setWidth(maxX + 1);
    rect.setHeight(maxY + 1);
    setPosition(rect);
  }

  void GenericControl::style(LayoutClasses layout)
  {
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
  }

  void GenericControl::onEventFired(std::any v, const ControlInstance::EventConnection &connection)
  {
    for(const auto &c : getControls())
    {
      if(auto a = std::dynamic_pointer_cast<Styleable>(c))
      {
        const auto &primitive = a->getPrimitive();

        if(primitive.primitiveInstance == connection.targetInstance)
        {
          if(auto p = std::dynamic_pointer_cast<PropertyOwner>(c))
          {
            p->setProperty(connection.targetProperty, v);
          }
        }
      }
    }
  }

    void GenericControl::setDirty() {
        ControlWithChildren::setDirty();
    }
}
