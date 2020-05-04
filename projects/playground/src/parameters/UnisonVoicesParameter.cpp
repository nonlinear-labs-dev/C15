#include "UnisonVoicesParameter.h"
#include <parameters/scale-converters/LinearCountScaleConverter.h>
#include <parameters/scale-converters/dimension/VoicesDimension.h>
#include <presets/EditBuffer.h>
#include <presets/PresetManager.h>
#include <presets/PresetParameter.h>

UnisonVoicesParameter::UnisonVoicesParameter(ParameterGroup *group, VoiceGroup vg)
    : UnmodulateableUnisonParameter(group, ParameterId { 249, vg },
                                    ScaleConverter::get<LinearCountScaleConverter<24, VoicesDimension>>(), 0, 23, 23)
{
}

bool UnisonVoicesParameter::shouldWriteDocProperties(UpdateDocumentContributor::tUpdateID knownRevision) const
{
  auto ret = Parameter::shouldWriteDocProperties(knownRevision) || didScalingChange();

  if(ret)
    m_scalingChanged = false;

  return ret;
}

void UnisonVoicesParameter::updateScaling(UNDO::Transaction *transaction, SoundType type)
{
  auto oldVoices = getDisplayValue();

  auto set24Voices = [&] {
    getValue().setScaleConverter(ScaleConverter::get<LinearCountScaleConverter<24, VoicesDimension>>());
    getValue().setCoarseDenominator(23);
    getValue().setFineDenominator(23);
  };

  auto set12Voices = [&] {
    getValue().setScaleConverter(ScaleConverter::get<LinearCountScaleConverter<12, VoicesDimension>>());
    getValue().setCoarseDenominator(11);
    getValue().setFineDenominator(11);
  };

  if(type == SoundType::Single)
  {
    transaction->addSimpleCommand([=](auto) { set24Voices(); }, [=](auto) { set12Voices(); });
  }
  else
  {
    transaction->addSimpleCommand([=](auto) { set12Voices(); }, [=](auto) { set24Voices(); });
  }

  setCPFromHwui(transaction, 0);
  stepCPFromHwui(transaction, oldVoices, {});

  m_scalingChanged = true;
  onChange();
}

bool UnisonVoicesParameter::didScalingChange() const
{
  return m_scalingChanged;
}

void UnisonVoicesParameter::copyFrom(UNDO::Transaction *transaction, const PresetParameter *other)
{
  if(!isLocked())
  {
    if(static_cast<EditBuffer *>(getParentGroup()->getParent())->isDual())
    {
      auto numVoicesIn = other->getValue() * 23.0;
      auto res = std::clamp(numVoicesIn / 11.0, 0.0, 1.0);
      loadFromPreset(transaction, res);
    }
    else
    {
      UnmodulateableUnisonParameter::copyFrom(transaction, other);
    }
  }
}

void UnisonVoicesParameter::copyTo(UNDO::Transaction *transaction, PresetParameter *other) const
{
  if(static_cast<EditBuffer *>(getParentGroup()->getParent())->isDual())
  {
    auto numVoicesIn = getControlPositionValue() * 11.0;
    auto res = numVoicesIn / 23.0;
    other->setValue(transaction, res);
  }
  else
  {
    UnmodulateableUnisonParameter::copyTo(transaction, other);
  }
}
