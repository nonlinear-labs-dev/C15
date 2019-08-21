#include <presets/EditBuffer.h>
#include <presets/PresetManager.h>
#include <Application.h>
#include "ConvertToSingleItem.h"

ConvertToSingle::ConvertToSingle()
    : BasicItem("Convert to Single")
{
}

void ConvertToSingle::doAction()
{
  Application::get().getPresetManager()->getEditBuffer()->setType(Type::Single);
}