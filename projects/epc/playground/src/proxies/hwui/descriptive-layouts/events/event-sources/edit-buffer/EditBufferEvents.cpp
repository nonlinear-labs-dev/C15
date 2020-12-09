#include "EditBufferEvents.h"
#include <Application.h>
#include <proxies/hwui/HWUI.h>
#include <groups/ParameterGroup.h>
#include <sigc++/sigc++.h>
#include <parameters/SplitPointParameter.h>
#include <presets/EditBuffer.h>
#include <parameter_declarations.h>
#include <device-settings/Settings.h>
#include <device-settings/SplitPointSyncParameters.h>

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
    default:
      return false;
    case SoundType::Layer:
    case SoundType::Split:
    {
      auto vgI = eb->getParameterGroupByID({ "Part", VoiceGroup::I })->isAnyParameterChanged();
      auto vgII = eb->getParameterGroupByID({ "Part", VoiceGroup::II })->isAnyParameterChanged();
      return vgI || vgII;
    }
  }
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
  auto muteI = eb->findParameterByID({ C15::PID::Voice_Grp_Mute, VoiceGroup::I })->getControlPositionValue() != 0;
  setValue({ muteI ? "\uE0BA" : "", 0 });
}

void DescriptiveLayouts::VGIIMuted::onChange(const EditBuffer *eb)
{
  auto muteII = eb->findParameterByID({ C15::PID::Voice_Grp_Mute, VoiceGroup::II })->getControlPositionValue() != 0;
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

template <typename... Args> bool anyControlPositionGreaterThanZero(const EditBuffer *eb, Args... args)
{
  return ((eb->findParameterByID(args)->getControlPositionValue() > 0) || ...);
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

bool DescriptiveLayouts::ToFXIOver0::check(const EditBuffer *eb)
{
  return eb->findParameterByID({ C15::PID::Out_Mix_To_FX, VoiceGroup::II })->getControlPositionValue() > 0;
}

void DescriptiveLayouts::ToFXIOver0::onChange(const EditBuffer *eb)
{
  setValue(check(eb));
}

bool DescriptiveLayouts::ToFXIIOver0::check(const EditBuffer *eb)
{
  return eb->findParameterByID({ C15::PID::Out_Mix_To_FX, VoiceGroup::I })->getControlPositionValue() > 0;
}

void DescriptiveLayouts::ToFXIIOver0::onChange(const EditBuffer *eb)
{
  setValue(check(eb));
}

bool DescriptiveLayouts::LayerIIFBFromI::check(const EditBuffer *eb)
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

  auto pfbl = anyControlPositionNotZero(eb, ParameterId { C15::PID::FB_Mix_Lvl, VoiceGroup::II });
  auto oscPM = anyControlPositionNotZero(eb, ParameterId { C15::PID::Osc_A_PM_FB, VoiceGroup::II },
                                         ParameterId { C15::PID::Osc_B_PM_FB, VoiceGroup::II });
  auto shaperFB = anyControlPositionGreaterThanZero(eb, ParameterId(C15::PID::Shp_A_FB_Mix, VoiceGroup::II),
                                                    ParameterId(C15::PID::Shp_B_FB_Mix, VoiceGroup::II));

  auto other = pfbl && (shaperFB || oscPM);
  auto state = comb || svf || fx;
  return (oscFB || state) && other;
}

bool DescriptiveLayouts::LayerIFBFromII::check(const EditBuffer *eb)
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

  auto pfbl = anyControlPositionNotZero(eb, ParameterId { C15::PID::FB_Mix_Lvl, VoiceGroup::I });
  auto oscPM = anyControlPositionNotZero(eb, ParameterId { C15::PID::Osc_A_PM_FB, VoiceGroup::I },
                                         ParameterId { C15::PID::Osc_B_PM_FB, VoiceGroup::I });
  auto shaperFB = anyControlPositionGreaterThanZero(eb, ParameterId(C15::PID::Shp_A_FB_Mix, VoiceGroup::I),
                                                    ParameterId(C15::PID::Shp_B_FB_Mix, VoiceGroup::I));

  auto other = pfbl && (shaperFB || oscPM);
  auto state = comb || svf || fx;
  return (oscFB || state) && other;
}

void DescriptiveLayouts::LayerFBState::onChange(const EditBuffer *eb)
{
  if(!LayerIFBFromII::check(eb) && LayerIIFBFromI::check(eb))
  {
    setValue("Layer_FB_B.png");
  }
  else if(LayerIFBFromII::check(eb) && !LayerIIFBFromI::check(eb))
  {
    setValue("Layer_FB_A.png");
  }
  else if(LayerIFBFromII::check(eb) && LayerIIFBFromI::check(eb))
  {
    setValue("Layer_FB_C.png");
  }
  else
  {
    setValue("");
  }
}

void DescriptiveLayouts::LayerFXState::onChange(const EditBuffer *eb)
{
  auto IToII = ToFXIIOver0::check(eb);
  auto IIToI = ToFXIOver0::check(eb);

  if(IToII && !IIToI)
  {
    setValue("Layer_FX_B.png");
  }
  else if(!IToII && IIToI)
  {
    setValue("Layer_FX_A.png");
  }
  else if(IToII && IIToI)
  {
    setValue("Layer_FX_C.png");
  }
  else
  {
    setValue("");
  }
}

void DescriptiveLayouts::LayerFXOffset::onChange(const EditBuffer *eb)
{
  auto IToII = ToFXIIOver0::check(eb);
  auto IIToI = ToFXIOver0::check(eb);

  if(!IIToI && IToII)
  {
    setValue({ 0, 2 });
  }
  else
  {
    setValue({ 0, 0 });
  }
}

void DescriptiveLayouts::SplitPointBehaviourIsDefaultWithoutSync::onChange(const EditBuffer *eb)
{
  auto sI = eb->findAndCastParameterByID<SplitPointParameter>({ C15::PID::Split_Split_Point, VoiceGroup::I });
  auto sync = Application::get().getSettings()->getSetting<SplitPointSyncParameters>();
  setValue((!sync->get()) && sI->inDefaultSplitBehaviour());
}

void DescriptiveLayouts::VGIIsMuted::onChange(const EditBuffer *eb)
{
  setValue(eb->findParameterByID({ C15::PID::Voice_Grp_Mute, VoiceGroup::I })->isValueDifferentFrom(0));
}

void DescriptiveLayouts::VGIIIsMuted::onChange(const EditBuffer *eb)
{
  setValue(eb->findParameterByID({ C15::PID::Voice_Grp_Mute, VoiceGroup::II })->isValueDifferentFrom(0));
}
