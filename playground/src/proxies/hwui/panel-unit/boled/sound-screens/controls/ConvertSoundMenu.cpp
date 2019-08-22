#include <Application.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/controls/Button.h>
#include "ConvertSoundMenu.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"

ConvertSoundMenu::ConvertSoundMenu(const Rect &rect)
    : ButtonMenu(rect, 2)
{
  setup();
}

void ConvertSoundMenu::convertSoundTo(Type newType)
{
  Application::get().getPresetManager()->getEditBuffer()->setType(newType);
  Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().bruteForce();
}

void ConvertSoundMenu::setup()
{
  clear();
  clearActions();

  auto type = Application::get().getPresetManager()->getEditBuffer()->getType();

  if(type != Type::Single)
    addButton("Single", std::bind(&ConvertSoundMenu::convertSoundTo, this, Type::Single));

  if(type != Type::Split)
    addButton("Split", std::bind(&ConvertSoundMenu::convertSoundTo, this, Type::Split));

  if(type != Type::Layer)
    addButton("Layer", std::bind(&ConvertSoundMenu::convertSoundTo, this, Type::Layer));

  sanitizeIndex();
  selectButton(getSelectedButton());
}
