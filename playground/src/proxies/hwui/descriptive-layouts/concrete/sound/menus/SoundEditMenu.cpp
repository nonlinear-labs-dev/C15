#include "SoundEditMenu.h"
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>

SoundEditMenu::SoundEditMenu(const Rect &r)
    : ScrollMenu(r)
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  m_connection = eb->onSoundTypeChanged(sigc::mem_fun(this, &SoundEditMenu::onTypeChanged));
}

void SoundEditMenu::onTypeChanged()
{
  Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().bruteForce();
}
