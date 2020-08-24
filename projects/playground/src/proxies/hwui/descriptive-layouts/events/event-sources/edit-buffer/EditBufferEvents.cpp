#include "EditBufferEvents.h"
#include <Application.h>
#include <proxies/hwui/HWUI.h>
#include <groups/ParameterGroup.h>
#include <sigc++/sigc++.h>
#include <parameters/SplitPointParameter.h>
#include <presets/EditBuffer.h>
#include <parameter_declarations.h>

void DescriptiveLayouts::EditBufferTypeStringEvent::onChange(const EditBuffer *eb)
{
  const auto type = eb->getType();
  const auto typeStr = toString(type);
  setValue({ typeStr + " Sound", 0 });
}

void DescriptiveLayouts::EditBufferName::onChange(const EditBuffer *eb)
{
  setValue({ eb->getName(), 0 });
}

void DescriptiveLayouts::EditBufferNameWithSuffix::onChange(const EditBuffer *eb)
{
  setValue({ eb->getNameWithSuffix(), 0 });
}

void DescriptiveLayouts::CurrentVoiceGroupLabel::onChange(const EditBuffer *eb)
{
  auto currentVG = Application::get().getHWUI()->getCurrentVoiceGroup();
  setValue({ eb->getVoiceGroupNameWithSuffix(currentVG, false), 0 });
}

void DescriptiveLayouts::IsCurrentVGI::onChange(const EditBuffer *eb)
{
  auto val = Application::get().getHWUI()->getCurrentVoiceGroup();
  setValue(val == VoiceGroup::I);
}

void DescriptiveLayouts::IsCurrentVGII::onChange(const EditBuffer *eb)
{
  auto val = Application::get().getHWUI()->getCurrentVoiceGroup();
  setValue(val == VoiceGroup::II);
}

void DescriptiveLayouts::SoundParamsButtonText::onChange(const EditBuffer *eb)
{
  auto base = "Part..";
  auto suffix = isChanged(eb) ? "*" : "";
  setValue({ std::string(base) + suffix, 0 });
}

bool DescriptiveLayouts::SoundParamsButtonText::isChanged(const EditBuffer *eb)
{
  auto type = eb->getType();
  switch(type)
  {
    case SoundType::Invalid:
    case SoundType::Single:
      return false;
    case SoundType::Layer:
    case SoundType::Split:
    {
      auto vgI = eb->getParameterGroupByID({ "Part", VoiceGroup::I })->isAnyParameterChanged();
      auto vgII = eb->getParameterGroupByID({ "Part", VoiceGroup::II })->isAnyParameterChanged();
      if(type == SoundType::Split)
        return vgI || vgII || (eb->getSplitPoint() != nullptr && eb->getSplitPoint()->isChangedFromLoaded());
      return vgI || vgII;
    }
  }
  return false;
}

void DescriptiveLayouts::SoundVoicesButtonText::onChange(const EditBuffer *eb)
{
  auto base = "Voices..";
  auto suffix = isChanged(eb) ? "*" : "";
  setValue({ std::string(base) + suffix, 0 });
}

bool DescriptiveLayouts::SoundVoicesButtonText::isChanged(const EditBuffer *eb)
{
  auto type = eb->getType();
  if(type == SoundType::Split)
  {
    auto monoI = eb->getParameterGroupByID({ "Mono", VoiceGroup::I });
    auto monoII = eb->getParameterGroupByID({ "Mono", VoiceGroup::II });
    auto vgUnisonI = eb->getParameterGroupByID({ "Unison", VoiceGroup::I });
    auto vgUnisonII = eb->getParameterGroupByID({ "Unison", VoiceGroup::II });
    return (monoI && monoI->isAnyParameterChanged()) || (monoII && monoII->isAnyParameterChanged())
        || (vgUnisonI && vgUnisonI->isAnyParameterChanged()) || (vgUnisonII && vgUnisonII->isAnyParameterChanged());
  }
  else if(type == SoundType::Layer || type == SoundType::Single)
  {
    auto mono = eb->getParameterGroupByID({ "Mono", VoiceGroup::I });
    auto vgUnison = eb->getParameterGroupByID({ "Unison", VoiceGroup::I });
    return (mono && mono->isAnyParameterChanged()) || (vgUnison && vgUnison->isAnyParameterChanged());
  }
  return false;
}

void DescriptiveLayouts::SoundMasterButtonText::onChange(const EditBuffer *eb)
{
  auto base = "Master..";
  auto suffix = isChanged(eb) ? "*" : "";
  setValue({ std::string(base) + suffix, 0 });
}

bool DescriptiveLayouts::SoundMasterButtonText::isChanged(const EditBuffer *eb)
{
  auto masterGroup = eb->getParameterGroupByID({ "Master", VoiceGroup::Global });
  auto scale = eb->getParameterGroupByID({ "Scale", VoiceGroup::Global });
  return (scale && scale->isAnyParameterChanged()) || (masterGroup && masterGroup->isAnyParameterChanged());
}

void DescriptiveLayouts::MonoButtonText::onChange(const EditBuffer *eb)
{
  auto base = "Mono..";
  auto suffix = isChanged(eb) ? "*" : "";
  setValue({ std::string(base) + suffix, 0 });
}

bool DescriptiveLayouts::MonoButtonText::isChanged(const EditBuffer *eb)
{
  switch(eb->getType())
  {
    case SoundType::Single:
    case SoundType::Invalid:
    case SoundType::Layer:
    {
      auto mono = eb->getParameterGroupByID({ "Mono", VoiceGroup::I });
      return mono && mono->isAnyParameterChanged();
    }
    case SoundType::Split:
    {
      auto mono = eb->getParameterGroupByID({ "Mono", Application::get().getHWUI()->getCurrentVoiceGroup() });
      return mono && mono->isAnyParameterChanged();
    }
  }
  return false;
}

void DescriptiveLayouts::UnisonButtonText::onChange(const EditBuffer *eb)
{
  auto base = "Unison..";
  auto suffix = isChanged(eb) ? "*" : "";
  setValue({ std::string(base) + suffix, 0 });
}

bool DescriptiveLayouts::UnisonButtonText::isChanged(const EditBuffer *eb)
{
  switch(eb->getType())
  {
    case SoundType::Single:
    case SoundType::Invalid:
    case SoundType::Layer:
    {
      auto unison = eb->getParameterGroupByID({ "Unison", Application::get().getHWUI()->getCurrentVoiceGroup() });
      return unison && unison->isAnyParameterChanged();
    }
    case SoundType::Split:
    {
      auto unison = eb->getParameterGroupByID({ "Unison", Application::get().getHWUI()->getCurrentVoiceGroup() });
      return unison && unison->isAnyParameterChanged();
    }
  }
  return false;
}

void DescriptiveLayouts::CurrentVoiceGroupText::onChange(VoiceGroup newSelection)
{
  setValue({ toString(newSelection), 0 });
}

void DescriptiveLayouts::VGIMuted::onChange(const EditBuffer *eb)
{
  auto muteI = eb->findParameterByID({ 395, VoiceGroup::I })->getControlPositionValue() != 0;
  setValue({ muteI ? "\uE0BA" : "", 0 });
}

void DescriptiveLayouts::VGIIMuted::onChange(const EditBuffer *eb)
{
  auto muteII = eb->findParameterByID({ 395, VoiceGroup::II })->getControlPositionValue() != 0;
  setValue({ muteII ? "\uE0BA" : "", 0 });
}

void DescriptiveLayouts::IsSingleSound::onChange(const EditBuffer *eb)
{
  setValue(eb->getType() == SoundType::Single);
}

void DescriptiveLayouts::IsLayerSound::onChange(const EditBuffer *eb)
{
  setValue(eb->getType() == SoundType::Layer);
}

void DescriptiveLayouts::IsSplitSound::onChange(const EditBuffer *eb)
{
  setValue(eb->getType() == SoundType::Split);
}

template <typename... Args> bool anyControlPositionNotZero(const EditBuffer *eb, Args... args)
{
  return ((eb->findParameterByID(args)->getControlPositionValue() != 0) || ...);
}

void DescriptiveLayouts::AnyLayerCrossFB::onChange(const EditBuffer *eb)
{
  setValue(anyControlPositionNotZero(
      eb, ParameterId(C15::PID::FB_Mix_Comb_Src, VoiceGroup::I), ParameterId(C15::PID::FB_Mix_Comb_Src, VoiceGroup::II),
      ParameterId(C15::PID::FB_Mix_SVF_Src, VoiceGroup::I), ParameterId(C15::PID::FB_Mix_SVF_Src, VoiceGroup::II),
      ParameterId(C15::PID::FB_Mix_FX_Src, VoiceGroup::I), ParameterId(C15::PID::FB_Mix_FX_Src, VoiceGroup::II)));
}

void DescriptiveLayouts::AnyLayerCrossFBFromII::onChange(const EditBuffer *eb)
{
  setValue(anyControlPositionNotZero(eb, ParameterId { C15::PID::FB_Mix_Comb_Src, VoiceGroup::II },
                                     ParameterId { C15::PID::FB_Mix_SVF_Src, VoiceGroup::II },
                                     ParameterId { C15::PID::FB_Mix_FX_Src, VoiceGroup ::II }));
}

void DescriptiveLayouts::AnyLayerCrossFBFromI::onChange(const EditBuffer *eb)
{
  setValue(anyControlPositionNotZero(eb, ParameterId { C15::PID::FB_Mix_Comb_Src, VoiceGroup::I },
                                     ParameterId { C15::PID::FB_Mix_SVF_Src, VoiceGroup::I },
                                     ParameterId { C15::PID::FB_Mix_FX_Src, VoiceGroup ::I }));
}

void DescriptiveLayouts::ToFXIUnder100::onChange(const EditBuffer *eb)
{
  setValue(eb->findParameterByID({ C15::PID::Out_Mix_To_FX, VoiceGroup::II })->getControlPositionValue() < 1.0);
}

void DescriptiveLayouts::ToFXIIUnder100::onChange(const EditBuffer *eb)
{
  setValue(eb->findParameterByID({ C15::PID::Out_Mix_To_FX, VoiceGroup::I })->getControlPositionValue() < 1.0);
}

void DescriptiveLayouts::ToFXIOver0::onChange(const EditBuffer *eb)
{
  setValue(eb->findParameterByID({ C15::PID::Out_Mix_To_FX, VoiceGroup::II })->getControlPositionValue() > 0);
}

void DescriptiveLayouts::ToFXIIOver0::onChange(const EditBuffer *eb)
{
  setValue(eb->findParameterByID({ C15::PID::Out_Mix_To_FX, VoiceGroup::I })->getControlPositionValue() > 0);
}

void DescriptiveLayouts::LayerIIFBFromI::onChange(const EditBuffer *eb)
{
  auto oscFB = anyControlPositionNotZero(eb, ParameterId { C15::PID::FB_Mix_Osc, VoiceGroup::II });

  auto combMix = anyControlPositionNotZero(eb, ParameterId { C15::PID::FB_Mix_Comb, VoiceGroup::II });
  auto combSrc = anyControlPositionNotZero(eb, ParameterId { C15::PID::FB_Mix_Comb_Src, VoiceGroup::II });
  auto comb = combMix && combSrc;

  auto svfMix = anyControlPositionNotZero(eb, ParameterId { C15::PID::FB_Mix_SVF, VoiceGroup::II });
  auto svfSrc = anyControlPositionNotZero(eb, ParameterId { C15::PID::FB_Mix_SVF_Src, VoiceGroup::II });
  auto svf = svfMix && svfSrc;

  auto fxMix = anyControlPositionNotZero(eb, ParameterId { C15::PID::FB_Mix_FX, VoiceGroup::II });
  auto fxSrc = anyControlPositionNotZero(eb, ParameterId { C15::PID::FB_Mix_FX_Src, VoiceGroup::II });
  auto fx = fxMix && fxSrc;

  auto state = comb || svf || fx;

  setValue(oscFB || state);
}

void DescriptiveLayouts::LayerIFBFromII::onChange(const EditBuffer *eb)
{
  auto oscFB = anyControlPositionNotZero(eb, ParameterId { C15::PID::FB_Mix_Osc, VoiceGroup::I });

  auto combMix = anyControlPositionNotZero(eb, ParameterId { C15::PID::FB_Mix_Comb, VoiceGroup::I });
  auto combSrc = anyControlPositionNotZero(eb, ParameterId { C15::PID::FB_Mix_Comb_Src, VoiceGroup::I });
  auto comb = combMix && combSrc;

  auto svfMix = anyControlPositionNotZero(eb, ParameterId { C15::PID::FB_Mix_SVF, VoiceGroup::I });
  auto svfSrc = anyControlPositionNotZero(eb, ParameterId { C15::PID::FB_Mix_SVF_Src, VoiceGroup::I });
  auto svf = svfMix && svfSrc;

  auto fxMix = anyControlPositionNotZero(eb, ParameterId { C15::PID::FB_Mix_FX, VoiceGroup::I });
  auto fxSrc = anyControlPositionNotZero(eb, ParameterId { C15::PID::FB_Mix_FX_Src, VoiceGroup::I });
  auto fx = fxMix && fxSrc;

  auto state = comb || svf || fx;

  setValue(oscFB || state);
}
