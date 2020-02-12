#include <Application.h>
#include <device-settings/PresetGlitchSuppression.h>
#include <proxies/lpc/LPCProxy.h>
#include <nltools/messaging/Message.h>
#include <proxies/audio-engine/AudioEngineProxy.h>

PresetGlitchSuppression::PresetGlitchSuppression(Settings &settings)
    : super(settings, true)
{
}

PresetGlitchSuppression::~PresetGlitchSuppression()
{
}

void PresetGlitchSuppression::sendToLPC() const
{
  Application::get().getLPCProxy()->sendSetting(PRESET_GLITCH_SUPPRESSION, get());

  nltools::msg::Setting::PresetGlitchMessage msg { get() };
  Application::get().getAudioEngineProxy()->sendSettingMessage<nltools::msg::Setting::PresetGlitchMessage>(msg);
}
