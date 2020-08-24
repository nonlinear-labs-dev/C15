#include <Application.h>
#include <device-settings/PresetGlitchSuppression.h>
#include <proxies/lpc/LPCProxy.h>
#include <nltools/messaging/Message.h>
#include <proxies/audio-engine/AudioEngineProxy.h>

PresetGlitchSuppression::PresetGlitchSuppression(UpdateDocumentContributor &settings)
    : super(settings, true)
{
}

PresetGlitchSuppression::~PresetGlitchSuppression()
{
}

void PresetGlitchSuppression::sendToLPC(SendReason reason) const
{
  nltools::msg::Setting::PresetGlitchMessage msg { get() };
  Application::get().getAudioEngineProxy()->sendSettingMessage<nltools::msg::Setting::PresetGlitchMessage>(msg);
}
