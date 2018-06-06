#include "GenericLayout.h"
#include "GenericControl.h"
#include "Bar.h"
#include "Border.h"
#include "Text.h"

#include "Application.h"
#include "proxies/hwui/controls/ButtonMenu.h"
#include "proxies/hwui/HWUI.h"

namespace DescriptiveLayouts
{
  GenericLayout::GenericLayout(const LayoutClass &prototype) :
      m_prototype(prototype)
  {
  }

  void GenericLayout::init()
  {
    super::init();
    createControls();
  }

  void GenericLayout::createControls()
  {
    for(auto &c : m_prototype.controls)
    {
      auto control = c.instantiate();
      if(auto g = dynamic_cast<GenericControl*>(control))
      {
        g->style(m_prototype.id);
        g->connect();
      }
      addControl(control);
    }
  }

  bool GenericLayout::onButton(Buttons i, bool down, ::ButtonModifiers modifiers)
  {
    if(down)
    {
      for(const EventSinkMapping &m : m_prototype.sinkMappings)
      {
        if(m.button == i)
        {
          if(!handleEventSink(m.sink))
          {
            EventSinkBroker::get().fire(m.sink);
          }
          return true;
        }
      }
    }
    return false;
  }

  bool GenericLayout::handleEventSink(EventSinks s)
  {
    switch(s)
    {
      case EventSinks::IncButtonMenu:
        if(auto m = findControlOfType<ButtonMenu>())
          if(Application::get().getHWUI()->getButtonModifiers()[ButtonModifier::SHIFT])
            m->antiToggle();
          else
            m->toggle();

        return true;

      case EventSinks::FireButtonMenu:
        if(auto m = findControlOfType<ButtonMenu>())
          m->doAction();

        return true;
    }

    return false;
  }

  bool GenericLayout::onRotary(int inc, ::ButtonModifiers modifiers)
  {
    while(inc > 0)
    {
      onButton(Buttons::ROTARY_PLUS, true, modifiers);
      inc--;
    }

    while(inc < 0)
    {
      onButton(Buttons::ROTARY_MINUS, true, modifiers);
      inc++;
    }
    return true;
  }
}
