#include <Application.h>
#include <device-settings/PitchbendOnPressedKey.h>
#include <proxies/lpc/LPCProxy.h>

PitchbendOnPressedKey::PitchbendOnPressedKey(Settings &settings)
    : super(settings, false)
{
}

PitchbendOnPressedKey::~PitchbendOnPressedKey()
{
}

void PitchbendOnPressedKey::sendToLPC() const
{
  Application::get().getLPCProxy()->sendSetting(PITCHBEND_ON_PRESSED_KEYS, get());
}
