#include <Application.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/controls/Button.h>
#include <nltools/Types.h>
#include "ConvertSoundMenu.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include <libundo/undo/Scope.h>

ConvertSoundMenu::ConvertSoundMenu(const Rect &rect)
    : ButtonMenu(rect, 2)
{
  setup();
}

void ConvertSoundMenu::convertSoundTo(SoundType newType)
{
  auto pm = Application::get().getPresetManager();
  SoundUseCases useCases(pm->getEditBuffer(), pm);

  switch(newType)
  {
    case SoundType::Single:
      useCases.convertToSingle(Application::get().getHWUI()->getCurrentVoiceGroup());
      break;
    case SoundType::Layer:
      useCases.convertToLayer();
      break;
    case SoundType::Split:
      useCases.convertToSplit();
      break;
  }
  Application::get().getHWUI()->setFocusAndMode(FocusAndMode { UIFocus::Sound, UIMode::Select, UIDetail::Init });
}

void ConvertSoundMenu::setup()
{
  clear();

  auto type = Application::get().getPresetManager()->getEditBuffer()->getType();

  if(type != SoundType::Single)
    addButton("Single", std::bind(&ConvertSoundMenu::convertSoundTo, this, SoundType::Single));

  if(type != SoundType::Split)
    addButton("Split", std::bind(&ConvertSoundMenu::convertSoundTo, this, SoundType::Split));

  if(type != SoundType::Layer)
    addButton("Layer", std::bind(&ConvertSoundMenu::convertSoundTo, this, SoundType::Layer));

  sanitizeIndex();
  selectButton(getSelectedButton());
}
