#include "DirectLoadSetting.h"
#include <device-settings/Settings.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/HWUI.h>

DirectLoadSetting::DirectLoadSetting(Settings &settings)
    : BooleanSetting(settings, false)
{
}

void DirectLoadSetting::load(const Glib::ustring &text, Initiator initiator)
{
  if(initiator == Initiator::EXPLICIT_LOAD)
  {
    m_isLoading = true;
    BooleanSetting::load(text, initiator);
    m_isLoading = false;
  }
  else
  {
    BooleanSetting::load(text, initiator);
  }
}

bool DirectLoadSetting::set(BooleanSettings m)
{
  auto ret = EnumSetting::set(m);

  if(!m_isLoading)
    if(auto pm = Application::get().getPresetManager())
      if(m == BooleanSettings::BOOLEAN_SETTING_TRUE)
      {
        auto hwui = Application::get().getHWUI();
        auto currentPart = hwui->getCurrentVoiceGroup();
        auto loadToPartData = hwui->getPresetPartSelection(currentPart);
        EditBufferUseCases useCase(pm->getEditBuffer());
        useCase.autoLoadSelectedPreset(currentPart, loadToPartData);
      }

  return ret;
}
