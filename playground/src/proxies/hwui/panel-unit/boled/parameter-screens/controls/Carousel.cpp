#include "Carousel.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"

Carousel::Carousel(const Rect &pos)
    : super(pos)
{
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::hide<0>(sigc::mem_fun(this, &Carousel::setup)));
}

Carousel::~Carousel()
{
}

void Carousel::antiTurn()
{
  turn();
}
