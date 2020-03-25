#include "EditBufferEvents.h"
#include <Application.h>
#include <proxies/hwui/HWUI.h>
#include <groups/ParameterGroup.h>
#include <sigc++/sigc++.h>
#include <parameters/SplitPointParameter.h>
#include <presets/EditBuffer.h>

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

void DescriptiveLayouts::CurrentVoiceGroupLabel::onChange(const EditBuffer *eb)
{
  auto currentVG = Application::get().getHWUI()->getCurrentVoiceGroup();
  setValue({ eb->getVoiceGroupNameWithSuffix(currentVG), 0 });
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
