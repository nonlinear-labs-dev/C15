#include <utility>

#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationCarousel.h>
#include <proxies/hwui/panel-unit/ButtonReceiver.h>
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
  GenericLayout::GenericLayout(LayoutClass prototype)
      : DFBLayout(Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled())
      , m_prototype(std::move(prototype))
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
      if(auto g = dynamic_cast<GenericControl *>(control))
      {
        g->style(m_prototype.id);
        g->connect();
      }
      addControl(control);
    }
  }

  template <class CB> bool traverse(Control *c, CB cb)
  {
    if(auto cc = dynamic_cast<ControlOwner *>(c))
    {
      for(auto &control : cc->getControls())
      {
        if(traverse(control.get(), cb))
          return true;
      }
    }

    if(auto receiver = dynamic_cast<ButtonReceiver *>(c))
    {
      return cb(receiver);
    }
    else
    {
      return false;
    }
  }

  bool GenericLayout::onButton(Buttons i, bool down, ::ButtonModifiers modifiers)
  {
    if(!down)
      removeButtonRepeat();

    for(auto &c : getControls())
    {
      if(traverse(c.get(), [=](ButtonReceiver *r) -> bool { return r->onButton(i, down, modifiers); }))
        return true;
    }

    if(down)
    {
      for(const EventSinkMapping &m : m_prototype.sinkMappings)
      {
        if(m.button == i)
        {
          if(!handleEventSink(m.sink))
          {
            if(m.repeat)
              installButtonRepeat([=]() { EventSinkBroker::get().fire(m.sink); });
            else
              EventSinkBroker::get().fire(m.sink);
          }
          return true;
        }
      }
    }
    return handleDefaults(i, down, modifiers);
  }

  bool GenericLayout::handleDefaults(Buttons buttons, bool down, ::ButtonModifiers modifiers)
  {
    if(down)
    {
      switch(buttons)
      {
        case Buttons::BUTTON_PRESET:
          togglePresetMode();
          return true;
        case Buttons::BUTTON_SOUND:
          toggleSoundMode();
          return true;
        case Buttons::BUTTON_SETUP:
          toggleSetupMode();
          return true;
        case Buttons::BUTTON_STORE:
          toggleStoreMode();
          return true;
        case Buttons::BUTTON_INFO:
          toggleInfo();
          return true;
        case Buttons::BUTTON_EDIT:
          toggleEdit();
          return true;
      }
    }
    return false;
  }

  bool GenericLayout::redrawLayout()
  {
    getOLEDProxy().clear();
    setAllDirty();
    return DFBLayout::redrawLayout();
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

  void GenericLayout::togglePresetMode()
  {
    auto *hwui = Application::get().getHWUI();
    auto current = hwui->getFocusAndMode();
    if(current.focus == UIFocus::Presets)
      hwui->setFocusAndMode({ UIFocus::Parameters, UIMode::Select, UIDetail::Init });
    else
      hwui->setFocusAndMode({ UIFocus::Presets, UIMode::Select, UIDetail::Init });
  }

  void GenericLayout::toggleSoundMode()
  {
    auto *hwui = Application::get().getHWUI();
    auto current = hwui->getFocusAndMode();
    if(current.focus == UIFocus::Sound)
      hwui->setFocusAndMode({ UIFocus::Parameters, UIMode::Select, UIDetail::Init });
    else
      hwui->setFocusAndMode({ UIFocus::Sound, UIMode::Select, UIDetail::Init });
  }

  void GenericLayout::toggleSetupMode()
  {
    auto *hwui = Application::get().getHWUI();
    auto current = hwui->getFocusAndMode();
    if(current.focus == UIFocus::Setup)
      hwui->setFocusAndMode({ UIFocus::Parameters, UIMode::Select, UIDetail::Init });
    else
      hwui->setFocusAndMode({ UIFocus::Setup, UIMode::Select, UIDetail::Init });
  }
  void GenericLayout::toggleStoreMode()
  {
    auto *hwui = Application::get().getHWUI();
    auto current = hwui->getFocusAndMode();
    if(current.focus == UIFocus::Presets && current.mode == UIMode::Store)
      hwui->setFocusAndMode({ UIFocus::Presets, UIMode::Select, UIDetail::Init });
    else
      hwui->setFocusAndMode({ UIFocus::Presets, UIMode::Store, UIDetail::Init });
  }
  void GenericLayout::toggleInfo()
  {
    auto *hwui = Application::get().getHWUI();
    auto current = hwui->getFocusAndMode();
    if(current.mode == UIMode::Info)
      hwui->setFocusAndMode({ UIFocus::Unchanged, UIMode::Select, UIDetail::Init });
    else
      hwui->setFocusAndMode({ UIFocus::Unchanged, UIMode::Info, UIDetail::Init });
  }
  void GenericLayout::toggleEdit()
  {

    auto *hwui = Application::get().getHWUI();
    auto current = hwui->getFocusAndMode();
    if(current.mode == UIMode::Edit)
      hwui->setFocusAndMode({ UIFocus::Unchanged, UIMode::Select, UIDetail::Init });
    else
      hwui->setFocusAndMode({ UIFocus::Unchanged, UIMode::Edit, UIDetail::Init });
  }
}
