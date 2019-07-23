#pragma once

#include <playground.h>
#include <proxies/hwui/FrameBuffer.h>
#include <tools/EnumTools.h>
#include <boost/serialization/strong_typedef.hpp>

namespace DescriptiveLayouts
{
  using ComponentID = Glib::ustring;

  ENUM(PrimitiveProperty, uint8_t, None, Text, Range, Visibility, Highlight, ControlPosition);
  ENUM(StyleKey, uint8_t, BackgroundColor, Color, TextAlign, FontSize, BorderStyle, FontDecoration, SuffixColor,
       BorderColor, HighlightBackgroundColor, HighlightColor);
  ENUM(ParameterType, uint8_t, Unipolar = 1 << 0, Bipolar = 1 << 1);
  ENUM(ButtonEvents, uint8_t, Down = true, Up = false);
  ENUM(ButtonModifiers, uint8_t, None = 0, Shift = 1 << 0);

  namespace StyleValues
  {
    ENUM(Font, uint8_t, Regular, Bold);
    ENUM(Color, int, Transparent = FrameBuffer::Transparent, C43 = FrameBuffer::C43, C77 = FrameBuffer::C77,
         C103 = FrameBuffer::C103, C128 = FrameBuffer::C128, C179 = FrameBuffer::C179, C204 = FrameBuffer::C204,
         C255 = FrameBuffer::C255);
    ENUM(Alignment, uint8_t, Left, Center, Right);
    ENUM(BorderStyle, uint8_t, Solid, Rounded, None);
  }

  ENUM(EventSources, uint8_t, Any, None, ParameterName, SliderRange, ParameterDisplayString, ParameterGroupName,
       IsBipolar, LockStatus, MacroControlSymbol, MacroControlAsignment, MacroControlAmount, MacroControlPosition,
       MacroControlPositionText, MCModRange, SoundHeaderText, EditBufferName, CurrentVoiceGroupName,
       ParameterControlPosition, ParameterNameWithStateSuffix, IsNotOnlyParameterOnButton, IsOnlyParameterOnButton,
       BooleanTrue, BooleanFalse, MCSelectButtonText, MCPositionButtonText, MCAmountButtonText, FullSoundName,
       isCurrentVGI, isCurrentVGII, SelectVGButtonText);

  ENUM(PrimitiveClasses, uint8_t, Any, Bar, Border, Text, Circle);

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
}
