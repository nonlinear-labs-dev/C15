#include "ParameterLayout.h"
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModuleCaption.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ParameterNameLabel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ParameterCarousel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/LockedIndicator.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/InvertedLabel.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/controls/ButtonMenu.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>

ParameterLayout2::ParameterLayout2 ()
{
  addControl (new ModuleCaption (Rect (0, 0, 64, 13)));
  addControl (new ParameterNameLabel (Rect (72, 8, 112, 11)));
  addControl (new LockedIndicator (Rect (68, 0, 4, 13)));
}

Parameter * ParameterLayout2::getCurrentParameter () const
{
  return Application::get ().getPresetManager ()->getEditBuffer ()->getSelected ();
}

Parameter * ParameterLayout2::getCurrentEditParameter () const
{
  return getCurrentParameter ();
}

bool ParameterLayout2::onButton (Buttons i, bool down, ButtonModifiers modifiers)
{
  if (down)
  {
    switch (i)
    {
    case Buttons::BUTTON_PRESET:
      Application::get ().getHWUI ()->undoableSetFocusAndMode (FocusAndMode (UIFocus::Presets, UIMode::Select));
      return true;

    case Buttons::BUTTON_STORE:
      Application::get ().getHWUI ()->undoableSetFocusAndMode (FocusAndMode (UIFocus::Presets, UIMode::Store));
      return true;

    case Buttons::BUTTON_INFO:
      Application::get ().getHWUI ()->undoableSetFocusAndMode (UIMode::Info);
      return true;

    case Buttons::BUTTON_DEFAULT:
      setDefault ();
      return true;
    }
  }

  return super::onButton (i, down, modifiers);
}

void ParameterLayout2::setDefault ()
{
  if (auto p = getCurrentEditParameter ())
    p->setDefaultFromHwui ();
}

bool ParameterLayout2::onRotary (int inc, ButtonModifiers modifiers)
{
  if (auto p = getCurrentEditParameter ())
  {
    auto scope = p->getUndoScope ().startContinuousTransaction (p, "Set '%0'", p->getGroupAndParameterName ());
    p->stepCPFromHwui (scope->getTransaction (), inc, modifiers);
    return true;
  }

  return super::onRotary (inc, modifiers);
}

ParameterSelectLayout2::ParameterSelectLayout2 () :
    super ()
{
}

void ParameterSelectLayout2::init ()
{
  super::init ();

  setCarousel(createCarousel (Rect (195, 0, 58, 64)));
  m_carousel->setHighlight (true);
}

Carousel *ParameterSelectLayout2::createCarousel (const Rect &rect)
{
  return new ParameterCarousel (rect);
}

void ParameterSelectLayout2::setCarousel(Carousel *c)
{
  if(m_carousel)
    remove(m_carousel);

  m_carousel = addControl (c);
}

Carousel *ParameterSelectLayout2::getCarousel()
{
  return m_carousel;
}

bool ParameterSelectLayout2::onButton (Buttons i, bool down, ButtonModifiers modifiers)
{
  if (down)
  {
    switch(i)
    {
      case Buttons::BUTTON_D:
        if(m_carousel)
        {
          if(modifiers[SHIFT] == 1)
          {
            m_carousel->antiTurn ();
          }
          else
          {
            m_carousel->turn ();
          }
        }

        return true;

      case Buttons::BUTTON_EDIT:
        Application::get ().getHWUI ()->undoableSetFocusAndMode (UIMode::Edit);
        return true;
    }
  }

  return super::onButton (i, down, modifiers);
}

ParameterEditLayout2::ParameterEditLayout2 () :
    super ()
{
  addControl (new InvertedLabel ("Edit", Rect (8, 26, 48, 12)))->setHighlight (true);
}

ParameterEditLayout2::~ParameterEditLayout2 ()
{
}

void ParameterEditLayout2::init ()
{
  super::init ();

  if ((m_menu = createMenu (Rect (195, 1, 58, 62))))
    addControl (m_menu);
}

ButtonMenu *ParameterEditLayout2::getMenu ()
{
  return m_menu;
}

bool ParameterEditLayout2::onButton (Buttons i, bool down, ButtonModifiers modifiers)
{
  if (down)
  {
    if (m_menu)
    {
      if (Buttons::BUTTON_D == i)
      {
        if(modifiers[SHIFT] == 1)
          m_menu->antiToggle();
        else
          m_menu->toggle ();
        return true;
      }

      if (Buttons::BUTTON_ENTER == i)
      {
        m_menu->doAction ();
        return true;
      }
    }

    if (Buttons::BUTTON_EDIT == i)
    {
      Application::get ().getHWUI ()->undoableSetFocusAndMode (UIMode::Select);
      return true;
    }
  }

  return super::onButton (i, down, modifiers);
}
