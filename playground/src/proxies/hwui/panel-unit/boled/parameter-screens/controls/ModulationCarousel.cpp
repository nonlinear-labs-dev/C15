#include <proxies/hwui/controls/ControlOwner.h>
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/CurrentModulatedValueLabel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/LowerModulationBoundControl.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationCarousel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/UpperModulationBoundControl.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ModulateableParameterLayouts.h>
#include "ModulationCarousel.h"
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/controls/Button.h>
#include <proxies/hwui/panel-unit/PanelUnit.h>

ModulationCarousel::ModulationCarousel(Mode mode, const Rect &pos)
    : super(pos)
    , m_modulationNotifier(this)
{
  m_upper = addControl(new UpperModulationBoundControl(Rect(0, 2, pos.getWidth(), 20)));
  m_upper->setHighlight(mode == Mode::UpperBound);
  m_middle = addControl(new CurrentModulatedValueLabel(Rect(0, 22, pos.getWidth(), 20)));
  m_middle->setHighlight(mode == Mode::ParameterValue);
  m_lower = addControl(new LowerModulationBoundControl(Rect(0, 42, pos.getWidth(), 20)));
  m_lower->setHighlight(mode == Mode::LowerBound);

  m_button = addControl(new Button("", Buttons::BUTTON_D));
  m_button->setHighlight(false);
}

void ModulationCarousel::setup(Parameter *p)
{
  if(auto modP = dynamic_cast<ModulateableParameter *>(p))
    onModulationSourceChanged(modP);
}

void ModulationCarousel::turn()
{
  bool found = false;

  for(const auto &c : getControls())
  {
    if(found)
    {
      c->setHighlight(true);
      return;
    }
    else if(c->isHighlight())
    {
      c->setHighlight(false);
      found = true;
    }
  }

  getControls().front()->setHighlight(true);
}

void ModulationCarousel::antiTurn()
{
  auto foundCtrl = *getControls().rbegin();

  for(const auto &c : getControls())
  {
    if(c->isHighlight())
    {
      foundCtrl->setHighlight(true);
      c->setHighlight(false);
      return;
    }
    foundCtrl = c;
  }
}

bool ModulationCarousel::onRotary(int inc, ButtonModifiers modifiers)
{
  for(const auto &c : getControls())
    if(c->isHighlight())
      if(auto a = std::dynamic_pointer_cast<RotaryEncoder::Receiver>(c))
        return a->onRotary(inc, modifiers);

  return false;
}

bool ModulationCarousel::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  if(!isVisible())
    return false;

  for(auto &c : getControls())
  {
    if(c->isHighlight())
    {
      if(auto buttonReceiver = dynamic_cast<ButtonReceiver *>(c.get()))
      {
        if(buttonReceiver->onButton(i, down, modifiers))
          return true;
      }
    }
  }
  return false;
}

void ModulationCarousel::onModulationSourceChanged(const ModulateableParameter *modP)
{
  auto visible = modP->getModulationSource() != ModulationSource::NONE;
  m_upper->setVisible(visible);
  m_lower->setVisible(visible);
  m_middle->setVisible(visible);
  m_button->setVisible(!visible);
}

bool ModulationCarousel::redraw(FrameBuffer &fb)
{
  auto ret = ControlWithChildren::redraw(fb);

  if(m_button->isVisible())
    ret |= m_button->redraw(fb);

  return ret;
}
