#include <presets/EditBuffer.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include "ConvertToSingleItem.h"

ConvertToSingleItem::ConvertToSingleItem() : AnimatedGenericItem("Convert to Single", [] {
  Application::get().getPresetManager()->getEditBuffer()->setType(Type::Single);
}) {
}
