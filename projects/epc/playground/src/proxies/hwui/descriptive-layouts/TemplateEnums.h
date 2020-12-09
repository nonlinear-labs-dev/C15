#pragma once

#include <playground.h>
#include <proxies/hwui/HWUIEnums.h>
#include <nltools/enums/EnumTools.h>
#include <proxies/hwui/buttons.h>

namespace DescriptiveLayouts
{
  using ComponentID = Glib::ustring;

  ENUM(PrimitiveProperty, uint8_t, None, Text, ImagePath, ImageOffset, ImageTransparency, Range, Visibility, Highlight,
       ControlPosition);
  ENUM(StyleKey, uint8_t, BackgroundColor, Color, TextAlign, FontSize, BorderStyle, FontDecoration, SuffixColor,
       BorderColor, HighlightBackgroundColor, HighlightColor);
  ENUM(ParameterType, uint8_t, Unipolar = 1 << 0, Bipolar = 1 << 1);
  ENUM(ButtonEvents, uint8_t, Down = true, Up = false);

  namespace StyleValues
  {
    ENUM(Font, uint8_t, Undefined, Regular, Bold);
    ENUM(Color, int, Undefined = static_cast<int>(FrameBufferColors::Undefined),
         Transparent = static_cast<int>(FrameBufferColors::Transparent), C43 = static_cast<int>(FrameBufferColors::C43),
         C77 = static_cast<int>(FrameBufferColors::C77), C103 = static_cast<int>(FrameBufferColors::C103),
         C128 = static_cast<int>(FrameBufferColors::C128), C179 = static_cast<int>(FrameBufferColors::C179),
         C204 = static_cast<int>(FrameBufferColors::C204), C255 = static_cast<int>(FrameBufferColors::C255));
    ENUM(Alignment, uint8_t, Undefined, Left, Center, Right);
    ENUM(BorderStyle, uint8_t, Undefined, Solid, Rounded, None);
  }

  ENUM(EventSources, uint8_t, Any, None, ParameterName, SliderRange, ParameterDisplayString, ParameterGroupName,
       IsBipolar, LockStatus, MacroControlSymbol, MacroControlAssignment, MacroControlAmount, MacroControlPosition,
       MacroControlPositionText, MCModRange, EditBufferTypeText, EditBufferName, ParameterControlPosition,
       ParameterNameWithStateSuffix, IsNotOnlyParameterOnButton, IsOnlyParameterOnButton, BooleanTrue, BooleanFalse,
       MCSelectionChanged, MCPositionChanged, MCAmountChanged, ParameterValueChanged, isCurrentVGI, isCurrentVGII,
       SelectVGButtonText, SoundEditHeading, DirectLoadStatus, PresetListBankName, PresetListPresetName, CanLeft,
       CanRight, isFineActive, EditBufferMasterVolume, EditBufferMasterTune, VGIMasterTuneText, VGIIMasterTuneText,
       VGIMasterVolumeText, VGIIMasterVolumeText, SplitPointValue, MonoEnabledText, MonoPrioText, MonoLegatoText,
       MonoGlideText, UnisonVoicesText, UnisonDetuneText, UnisonPhaseText, UnisonPanText, MonoEnabled, UnisonEnabled,
       CurrentVoiceGroupLabel, SoundParamsButtonText, SoundVoicesButtonText, SoundMasterButtonText, MonoButtonText,
       UnisonButtonText, VGIMuted, VGIIMuted, VGIIsMuted, VGIIIsMuted, IsSingleSound, IsLayerSound, IsSplitSound,
       SplitPointIValue, SplitPointIIValue,

       // try to use more generic names, the specific meaning is implemented in the EventProvider
       Edit, Fine, Locked, Position, NumItems,

       ButtonA, ButtonB, ButtonC, ButtonD,

       PreviousNumber, PreviousName, PreviousSelected, PreviousLoaded,

       CurrentNumber, CurrentName, CurrentSelected, CurrentLoaded,

       NextNumber, NextName, NextSelected, NextLoaded, PreviousType, NextType, CurrentType, CurrentVoiceGroupText,
       EditBufferNameWithSuffix, SyncSplitEnabled, SyncSplitDisabled, SplitPointDefaultBehaviourWithoutSync,

       AnyLayerCrossFB, AnyLayerCrossFBToI, AnyLayerCrossFBToII, ToFXIIUnder100, ToFXIUnder100, ToFXIIOver0, ToFXIOver0,
       LayerFBState, LayerFXState, LayerFXOffset);

  ENUM(EventSinks, uint8_t, Swallow, SwitchToInitDetail, SwitchToEditMode, SwitchToSelectMode, SwitchToSetupFocus,
       SwitchToParameterFocus, SwitchToBankFocus, SwitchToPresetFocus, SwitchToSoundFocus, SwitchToMCSelectDetail,
       SwitchToButtonADetail, SwitchToButtonBDetail, SwitchToButtonCDetail, SwitchToButtonDDetail,
       SwitchToMCModRangeDetail, SwitchToMCAmtDetail, IncMCSel, DecMCSel, IncMCAmt, DecMCAmt, IncMCPos, DecMCPos,
       IncButtonMenu, DecButtonMenu, FireButtonMenu, IncModulationCarousel, DecModulationCarousel, ToggleVoiceGroup,
       ToggleVoiceGroupWithParameterSelection, CommitPresetForButtonMenu, ToggleDirectLoad,
       IncPresetSelectionPresetList, DecPresetSelectionPresetList, IncBankSelectionPresetList,
       DecBankSelectionPresetList, DoPresetListAction, OpenUnisonParameter, OpenMasterParameter, SwitchToVoicesDetail,
       OpenMonoParameterScreen, OpenPartScreen, InitSound, IncSplitPoint, DecSplitPoint, LayerMuteInc, LayerMuteDec,

       // try to use more generic names, the specific meaning is implemented in the EventProvider
       Left, Right, Up, Down, IncParam, DecParam, Commit);

  ENUM(EventProviders, uint8_t, None, Global, IndependentPresetSelectionEvents, PresetManagerEvents);
  ENUM(PrimitiveClasses, uint8_t, Any, Bar, Border, Text, Circle, Image, Concrete);

  template <typename Derived> class StringId : public std::string
  {
    using super = std::string;

   public:
    StringId() = default;

    constexpr StringId(const super &i)
        : super(i)
    {
    }

    StringId(const char *i)
        : super(i)
    {
    }

    static const StringId<Derived> None;
    static const StringId<Derived> Any;
  };

  template <typename Derived> const StringId<Derived> StringId<Derived>::Any = "ANY";
  template <typename Derived> const StringId<Derived> StringId<Derived>::None = "NONE";

  namespace traits
  {
    namespace Primitives
    {
      struct Instance
      {
      };

      struct Tag
      {
      };

      struct Text
      {
      };
    }

    namespace Controls
    {
      struct Instance
      {
      };
      struct Class
      {
      };
    }

    namespace Layouts
    {
      struct Class
      {
      };
    }
  }

  using PrimitiveTag = StringId<traits::Primitives::Tag>;
  using PrimitiveInstances = StringId<traits::Primitives::Instance>;
  using ControlClasses = StringId<traits::Controls::Class>;
  using ControlInstances = StringId<traits::Controls::Instance>;
  using LayoutClasses = StringId<traits::Layouts::Class>;

  struct EventSinkMapping
  {
   public:
    EventSinkMapping(Buttons button, EventSinks sink, ButtonEvents event = ButtonEvents::Down, bool rep = false)
        : button(button)
        , sink(sink)
        , event(event)
        , repeat(rep)
    {
    }

    Buttons button;
    ButtonEvents event;
    EventSinks sink;
    bool repeat;
  };
}
