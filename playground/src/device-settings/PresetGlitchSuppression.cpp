#include <Application.h>
#include <device-settings/PresetGlitchSuppression.h>
#include <proxies/lpc/LPCProxy.h>

PresetGlitchSuppression::PresetGlitchSuppression(Settings &settings)
    : super(settings, false)
{
}

PresetGlitchSuppression::~PresetGlitchSuppression()
{
}

void PresetGlitchSuppression::sendToLPC() const
{
  Application::get().getLPCProxy()->sendSetting(PRESET_GLITCH_SUPPRESSION, get());
}
