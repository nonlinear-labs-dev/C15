#pragma once

#include <bitset>
#include <nltools/enums/EnumTools.h>

ENUM(UIFocus, uint8_t, Any, Sound, Parameters, Presets, Banks, Setup, Unchanged);
ENUM(UIMode, uint8_t, Any, Select, Store, Edit, Info, Unchanged);
ENUM(UIDetail, uint8_t, Any, Unchanged, Init, ButtonA, ButtonB, ButtonC, ButtonD, MCSelect, MCAmount, MCPosition,
     MCModRange, Voices, SoundParameters);
ENUM(LayoutVersionMode, uint8_t, Old, Mixed, New);

enum ButtonModifier
{
  FINE,
  SHIFT,
  NUM_MODIFIERS
};

enum class FrameBufferColors : uint8_t
{
  Undefined = 0xFF,
  Transparent = 0x10,
  C43 = 0x00,
  C77 = 0x02,
  C103 = 0x05,
  C128 = 0x06,
  C179 = 0x0A,
  C204 = 0x0B,
  C255 = 0x0F,
  SYNC = 0xFF
};

class ButtonModifiers : public std::bitset<ButtonModifier::NUM_MODIFIERS>
{
  typedef std::bitset<ButtonModifier::NUM_MODIFIERS> super;

 public:
  ButtonModifiers()
  {
  }

  ButtonModifiers(unsigned long long dontCallme);

  ButtonModifiers(ButtonModifier mod)
  {
    set(mod);
  }
};

struct FocusAndMode
{
  FocusAndMode(UIFocus f, UIMode m)
      : focus(f)
      , mode(m)
      , detail(UIDetail::Init)
  {
  }

  FocusAndMode(UIFocus f, UIMode m, UIDetail d)
      : focus { f }
      , mode { m }
      , detail { d }
  {
  }

  FocusAndMode()
      : FocusAndMode(UIFocus::Parameters)
  {
  }

  FocusAndMode(UIFocus f)
      : FocusAndMode(f, UIMode::Unchanged)
  {
  }

  FocusAndMode(UIMode m)
      : FocusAndMode(UIFocus::Unchanged, m)
  {
  }

  bool operator==(const FocusAndMode &other) const
  {
    return other.focus == focus && other.mode == mode && other.detail == detail;
  }

  bool operator!=(const FocusAndMode &other) const
  {
    return !(*this == other);
  }

  void fixUnchanged(const FocusAndMode &other)
  {
    if(focus == UIFocus::Unchanged)
      focus = other.focus;

    if(mode == UIMode::Unchanged)
      mode = other.mode;

    if(detail == UIDetail::Unchanged)
      detail = other.detail;
  }

  UIFocus focus = UIFocus::Unchanged;
  UIMode mode = UIMode::Unchanged;
  UIDetail detail = UIDetail::Unchanged;

  std::string toString()
  {
    return "UIFocus: " + ::toString(focus) + " UIMode: " + ::toString(mode) + " UIDetail: " + ::toString(detail);
  }
};
