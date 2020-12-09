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

  class VGIIsMuted : public EditBufferEvent<bool>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };

  class VGIIIsMuted : public EditBufferEvent<bool>
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

  class AnyLayerCrossFBFromII : public EditBufferEvent<bool>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };

  class AnyLayerCrossFBFromI : public EditBufferEvent<bool>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };

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

  class LayerIFBFromII
  {
   public:
    static bool check(const EditBuffer *eb);
  };

  class LayerIIFBFromI
  {
   public:
    static bool check(const EditBuffer *eb);
  };

  class ToFXIOver0 : public EditBufferEvent<bool>
  {
   public:
    void onChange(const EditBuffer *eb) override;
    static bool check(const EditBuffer *eb);
  };

  class ToFXIIOver0 : public EditBufferEvent<bool>
  {
   public:
    void onChange(const EditBuffer *eb) override;
    static bool check(const EditBuffer *eb);
  };

  class LayerFBState : public EditBufferEvent<std::string>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };

  class LayerFXState : public EditBufferEvent<std::string>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };

  class LayerFXOffset : public EditBufferEvent<std::pair<int, int>>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };

  class SplitPointBehaviourIsDefaultWithoutSync : public EditBufferEvent<bool>
  {
   public:
    void onChange(const EditBuffer *eb) override;
  };
}
