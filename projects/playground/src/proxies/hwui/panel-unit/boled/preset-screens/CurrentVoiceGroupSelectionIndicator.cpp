#include "CurrentVoiceGroupSelectionIndicator.h"
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>
#include <sigc++/sigc++.h>

inline static Glib::ustring currentVoiceGroupToString()
{
  auto currentVG = Application::get().getHWUI()->getCurrentVoiceGroup();
  return toString(currentVG);
}

CurrentVoiceGroupSelectionIndicator::CurrentVoiceGroupSelectionIndicator(const Rect &r)
    : LabelRegular8(currentVoiceGroupToString(), r)
{
  Application::get().getHWUI()->onCurrentVoiceGroupChanged(
      sigc::hide(sigc::mem_fun(this, &CurrentVoiceGroupSelectionIndicator::focusChanged)));

  Application::get().getPresetManager()->getEditBuffer()->onPresetLoaded(
      sigc::mem_fun(this, &CurrentVoiceGroupSelectionIndicator::focusChanged));

  Application::get().getPresetManager()->getEditBuffer()->onSoundTypeChanged(
      sigc::hide(sigc::mem_fun(this, &CurrentVoiceGroupSelectionIndicator::focusChanged)));
}

void CurrentVoiceGroupSelectionIndicator::focusChanged()
{
  if(Application::get().getPresetManager()->getEditBuffer()->getType() != SoundType::Single)
    setText({ currentVoiceGroupToString(), 0 });
  else
    setText({ "", 0 });
}
