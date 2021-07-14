#include <proxies/hwui/controls/ControlOwner.h>
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/CurrentModulatedValueLabel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/LowerModulationBoundControl.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationCarousel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/UpperModulationBoundControl.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ModulateableParameterLayouts.h>

ModulationCarousel::ModulationCarousel(Mode mode, const Rect &pos)
    : super(pos)
    , m_mode { mode }
{
  addControl(new UpperModulationBoundControl(Rect(0, 1, pos.getWidth(), 20)))->setHighlight(m_mode == Mode::UpperBound);
  addControl(new CurrentModulatedValueLabel(Rect(0, 21, pos.getWidth(), 20)))
      ->setHighlight(m_mode == Mode::ParameterValue);
  addControl(new LowerModulationBoundControl(Rect(0, 41, pos.getWidth(), 20)))
      ->setHighlight(m_mode == Mode::LowerBound);
}

void ModulationCarousel::setup(Parameter *p)
{
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

ModulationCarousel::Mode ModulationCarousel::getMode() const
{
  return m_mode;
}