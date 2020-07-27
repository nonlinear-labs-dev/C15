#pragma once
#include <presets/EditBuffer.h>
#include <parameter_declarations.h>
#include "EditBufferEvent.h"

namespace DescriptiveLayouts
{
  class EditBufferTypeStringEvent : public EditBufferEvent<DisplayString>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };

  class EditBufferName : public EditBufferEvent<DisplayString>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };

  class EditBufferNameWithSuffix : public EditBufferEvent<DisplayString>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };

  class CurrentVoiceGroupLabel : public EditBufferEvent<DisplayString>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };

  class IsCurrentVGI : public EditBufferEvent<bool>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };

  class IsCurrentVGII : public EditBufferEvent<bool>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };

  class VGIMuted : public EditBufferEvent<DisplayString>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };

  class VGIIMuted : public EditBufferEvent<DisplayString>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };

  class IsSingleSound : public EditBufferEvent<bool>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };

  class IsLayerSound : public EditBufferEvent<bool>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };

  class IsSplitSound : public EditBufferEvent<bool>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };

  class CurrentVoiceGroupText : public VoiceGroupSelectedEvent<DisplayString>
  {
   public:
    void onChange(VoiceGroup newSelection) override;
  };

  class SoundParamsButtonText : public EditBufferEvent<DisplayString>
  {
   public:
    void onChange(const EditBuffer *eb) override;

   private:
    bool isChanged(const EditBuffer *eb);
  };

  class SoundVoicesButtonText : public EditBufferEvent<DisplayString>
  {
   public:
    void onChange(const EditBuffer *eb) override;

   private:
    bool isChanged(const EditBuffer *eb);
  };

  class SoundMasterButtonText : public EditBufferEvent<DisplayString>
  {
   public:
    void onChange(const EditBuffer *eb) override;

   private:
    bool isChanged(const EditBuffer *eb);
  };

  class MonoButtonText : public EditBufferEvent<DisplayString>
  {
   public:
    void onChange(const EditBuffer *eb) override;

   private:
    bool isChanged(const EditBuffer *eb);
  };

  class UnisonButtonText : public EditBufferEvent<DisplayString>
  {
   public:
    void onChange(const EditBuffer *eb) override;

   private:
    bool isChanged(const EditBuffer *eb);
  };

  class AnyLayerCrossFB : public EditBufferEvent<bool>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };

  class AnyLayerCrossFBToI : public EditBufferEvent<bool>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };

  class AnyLayerCrossFBToII : public EditBufferEvent<bool>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };

  //ToFXIIUnder100, ToFXIUnder100, ToFXIIOver0, ToFXIOver0

  class ToFXIUnder100 : public EditBufferEvent<bool>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };

  class ToFXIIUnder100 : public EditBufferEvent<bool>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };

  template <VoiceGroup vg> class LayerOwnFB : public EditBufferEvent<bool>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };

  template <VoiceGroup vg> void DescriptiveLayouts::LayerOwnFB<vg>::onChange(const EditBuffer *eb)
  {
    //ungleich 0: FBMixer Comb, SVF, Effects
    // From I ungleich 100%
    auto fbComb = eb->findParameterByID({ C15::PID::FB_Mix_Comb, vg });
    auto svf = eb->findParameterByID({ C15::PID::FB_Mix_SVF, vg });
    auto effects = eb->findParameterByID({ C15::PID::FB_Mix_FX, vg });
    auto fromComb = eb->findParameterByID({ C15::PID::FB_Mix_Comb_Src, vg });
    auto fromSvf = eb->findParameterByID({ C15::PID::FB_Mix_SVF_Src, vg });
    auto fromFX = eb->findParameterByID({ C15::PID::FB_Mix_FX_Src, vg });

    auto unequalsZero = [&](auto p) { return p->getControlPositionValue() != 0; };

    auto unequals100 = [&](auto p) { return p->getControlPositionValue() != 1; };

    auto val = unequalsZero(fbComb) || unequalsZero(svf) || unequalsZero(effects);
    auto val2 = unequals100(fromComb) || unequals100(fromSvf) || unequals100(fromFX);
    setValue(val || val2);
  }

  class LayerIFBToII : public EditBufferEvent<bool>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };

  class LayerIIFBToI : public EditBufferEvent<bool>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };

  class ToFXIOver0 : public EditBufferEvent<bool>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };

  class ToFXIIOver0 : public EditBufferEvent<bool>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };
}
