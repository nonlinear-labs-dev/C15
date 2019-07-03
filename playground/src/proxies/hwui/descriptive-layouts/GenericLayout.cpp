#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationCarousel.h>
#include "GenericLayout.h"
#include "GenericControl.h"
#include "proxies/hwui/descriptive-layouts/Primitives/Bar.h"
#include "proxies/hwui/descriptive-layouts/Primitives/Border.h"
#include "proxies/hwui/descriptive-layouts/Primitives/Text.h"

#include "Application.h"
#include "proxies/hwui/controls/ButtonMenu.h"
#include "proxies/hwui/HWUI.h"

namespace DescriptiveLayouts
{

  struct ConditionResult
  {
    explicit ConditionResult(const LayoutClass::ConditionList &c)
        : m_conditions{ c }
    {
      for(auto &condition : m_conditions)
      {
        m_result.push_back(condition());
      }
      assert(m_result.size() == m_conditions.size());
    }

    bool hasChanged() const
    {
      std::vector<bool> res;
      for(auto &c : m_conditions)
      {
        res.push_back(c());
      }

      for(auto i = 0; i < res.size(); i++)
      {
        if(res[i] != m_result[i])
          return false;
      }

      return true;
    }

    LayoutClass::ConditionList m_conditions;
    std::vector<bool> m_result;
  };

  GenericLayout::GenericLayout(const LayoutClass &prototype)
      : DFBLayout(Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled())
      , m_prototype(prototype)
  {

    //TODO change to Throttled Job + more sophisticated evaluation process
    ConditionResult result{ prototype.conditions };

    Application::get().getMainContext()->signal_idle().connect([result]() {
      if(result.hasChanged())
      {
        const auto &hwui = Application::get().getHWUI();
        hwui->getPanelUnit().getEditPanel().getBoled().setupFocusAndMode(
            Application::get().getHWUI()->getFocusAndMode());
        return false;
      }
      return true;
    });
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
      if(auto g = dynamic_cast<GenericControl *>(control))
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
        {
          if(Application::get().getHWUI()->getButtonModifiers()[ButtonModifier::SHIFT])
          {
            m->antiToggle();
          }
          else
          {
            m->toggle();
          }
          return true;
        }
        if(auto car = findControlOfType<ModulationCarousel>())
        {
          if(Application::get().getHWUI()->getButtonModifiers()[ButtonModifier::SHIFT])
          {
            car->antiTurn();
          }
          else
          {
            car->turn();
          }
          return true;
        }

      case EventSinks::DecButtonMenu:
        if(auto m = findControlOfType<ButtonMenu>())
        {
          if(Application::get().getHWUI()->getButtonModifiers()[ButtonModifier::SHIFT])
          {
            m->toggle();
          }
          else
          {
            m->antiToggle();
          }
          return true;
        }
        if(auto car = findControlOfType<ModulationCarousel>())
        {
          if(Application::get().getHWUI()->getButtonModifiers()[ButtonModifier::SHIFT])
          {
            car->turn();
          }
          else
          {
            car->antiTurn();
          }
          return true;
        }

      case EventSinks::FireButtonMenu:
        if(auto m = findControlOfType<ButtonMenu>())
        {
          m->doAction();
          return true;
        }
      case EventSinks::IncParam:
        if(auto car = findControlOfType<ModulationCarousel>())
        {
          car->onRotary(1, Application::get().getHWUI()->getButtonModifiers());
          return true;
        }
      case EventSinks::DecParam:
        if(auto car = findControlOfType<ModulationCarousel>())
        {
          car->onRotary(-1, Application::get().getHWUI()->getButtonModifiers());
          return true;
        }
      default:
        return false;
    }
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
