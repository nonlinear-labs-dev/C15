#include <Application.h>
#include "ConvertSoundMenu.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"

ConvertSoundMenu::ConvertSoundMenu(const Rect &rect)
    : ButtonMenu(rect, 2)
{
  setup();
}

void ConvertSoundMenu::convertSoundTo(EditBuffer::Type newType)
{
    Application::get().getPresetManager()->getEditBuffer()->setType(newType);
    setup();
}

void ConvertSoundMenu::setup()
{
  clear();
  clearActions();

  auto type = Application::get().getPresetManager()->getEditBuffer()->getType();

  if(type != EditBuffer::Type::Single)
    addButton("Single", std::bind(&ConvertSoundMenu::convertSoundTo, this, EditBuffer::Type::Single));

  if(type != EditBuffer::Type::Split)
    addButton("Split", std::bind(&ConvertSoundMenu::convertSoundTo, this, EditBuffer::Type::Split));

  if(type != EditBuffer::Type::Layer)
    addButton("Layer", std::bind(&ConvertSoundMenu::convertSoundTo, this, EditBuffer::Type::Layer));

  sanitizeIndex();
  selectButton(getSelectedButton());
}
