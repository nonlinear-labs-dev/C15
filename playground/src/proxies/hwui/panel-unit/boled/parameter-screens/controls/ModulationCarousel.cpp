#include <proxies/hwui/controls/ControlOwner.h>
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/CurrentModulatedValueLabel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/LowerModulationBoundControl.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationCarousel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/UpperModulationBoundControl.h>

ModulationCarousel::ModulationCarousel(Mode mode, const Rect &pos)
    : super(pos)
{
  addControl(new UpperModulationBoundControl(Rect(0, 0, pos.getWidth(), 21)))->setHighlight(mode == Mode::UpperBound);
  addControl(new CurrentModulatedValueLabel(Rect(0, 21, pos.getWidth(), 20)))
      ->setHighlight(mode == Mode::ParameterValue);
  addControl(new LowerModulationBoundControl(Rect(0, 41, pos.getWidth(), 21)))->setHighlight(mode == Mode::LowerBound);
}

ModulationCarousel::~ModulationCarousel()
{
}

void ModulationCarousel::setup(Parameter *p)
{
}

void ModulationCarousel::turn()
{
  bool found = false;

  for(auto c : getControls())
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

  for(auto c : getControls())
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
  for(auto c : getControls())
    if(c->isHighlight())
      if(auto a = dynamic_pointer_cast<RotaryEncoder::Receiver>(c))
        return a->onRotary(inc, modifiers);

  return false;
}
