#include <Application.h>
#include <io/ToLPCBridge.h>
#include "Ribbon.h"
#include <io/Bridges.h>
#include <io/FromLPCBridge.h>

<<<<<<< HEAD
Ribbon::Ribbon()
    : m_upperRibbon(true)
{
=======
Ribbon::Ribbon() : m_upperRibbon(true) {
>>>>>>> layouts-reloaded
  set_draw_value(false);
  set_digits(0);
  set_range(0, 16000);
}

<<<<<<< HEAD
bool Ribbon::isUpperRibbon()
{
  return true;
}

void Ribbon::on_value_changed()
{
  auto b = Application::get().getBridges()->getBridge<FromLPCBridge>();
  b->sendRibbonPosition(isUpperRibbon(), get_value());
}
=======
bool Ribbon::isUpperRibbon() {
  return true;
}

void Ribbon::on_value_changed() {
  auto b = Application::get().getBridges()->getBridge<FromLPCBridge>();
  b->sendRibbonPosition(isUpperRibbon(), get_value());
}

>>>>>>> layouts-reloaded
