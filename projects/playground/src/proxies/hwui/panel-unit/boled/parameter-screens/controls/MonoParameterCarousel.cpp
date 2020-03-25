#include "MonoParameterCarousel.h"
#include <Application.h>
#include <proxies/hwui/HWUI.h>

std::vector<ParameterId> MonoParameterCarousel::getParameterIdsForMode(SoundType type)
{
  auto currentVG = Application::get().getHWUI()->getCurrentVoiceGroup();

  switch(type)
  {
    case SoundType::Split:
      return { { 364, currentVG }, { 365, currentVG }, { 367, currentVG }, { 366, currentVG } };
    case SoundType::Layer:
    case SoundType::Single:
    default:
      return { { 364, VoiceGroup::I }, { 365, VoiceGroup::I }, { 367, VoiceGroup::I }, { 366, VoiceGroup::I } };
  }

  return {};
}
