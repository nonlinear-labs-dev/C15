#pragma once

#include "nltools/enums/EnumTools.h"
#include <nltools/GlibFwd.h>
#include <limits>
#include <type_traits>

ENUM(SoundType, int8_t, Single, Split, Layer, Invalid);
ENUM(MacroControls, int8_t, NONE = 0, MC1 = 1, MC2 = 2, MC3 = 3, MC4 = 4, MC5 = 5, MC6 = 6, NUM_CHOICES = 7);
ENUM(VoiceGroup, int8_t, I, II, Global, NumGroups);
ENUM(RibbonTouchBehaviour, int8_t, ABSOLUTE, RELATIVE, NUM_TOUCH_BEHAVIOURS);
ENUM(RibbonReturnMode, int8_t, STAY = 0, RETURN = 1, NUM_RETURN_MODES = 2);
ENUM(PedalModes, int8_t, STAY = 0, RETURN_TO_ZERO = 1, RETURN_TO_CENTER = 2, NUM_PEDAL_MODES = 3);
ENUM(ReturnMode, int8_t, None, Center, Zero);

ENUM(MidiReceiveChannel, int8_t, None, Omni, CH_1, CH_2, CH_3, CH_4, CH_5, CH_6, CH_7, CH_8, CH_9, CH_10, CH_11, CH_12,
     CH_13, CH_14, CH_15, CH_16)

ENUM(MidiReceiveChannelSplit, int8_t, None, Omni, CH_1, CH_2, CH_3, CH_4, CH_5, CH_6, CH_7, CH_8, CH_9, CH_10, CH_11,
     CH_12, CH_13, CH_14, CH_15, CH_16, Common)

ENUM(MidiSendChannel, int8_t, None, CH_1, CH_2, CH_3, CH_4, CH_5, CH_6, CH_7, CH_8, CH_9, CH_10, CH_11, CH_12, CH_13,
     CH_14, CH_15, CH_16)

ENUM(MidiSendChannelSplit, int8_t, None, CH_1, CH_2, CH_3, CH_4, CH_5, CH_6, CH_7, CH_8, CH_9, CH_10, CH_11, CH_12,
     CH_13, CH_14, CH_15, CH_16, Common)

ENUM(RibbonCC, int8_t, None, CC01, CC02, CC03, CC04, CC05, CC06, CC07, CC08, CC09, CC10, CC11, CC12, CC13, CC14, CC15,
     CC16, CC17, CC18, CC19, CC20, CC21, CC22, CC23, CC24, CC25, CC26, CC27, CC28, CC29, CC30, CC31);

ENUM(PedalCC, int8_t, None, CC01, CC02, CC03, CC04, CC05, CC06, CC07, CC08, CC09, CC10, CC11, CC12, CC13, CC14, CC15,
     CC16, CC17, CC18, CC19, CC20, CC21, CC22, CC23, CC24, CC25, CC26, CC27, CC28, CC29, CC30, CC31, CC64, CC65, CC66,
     CC67, CC68, CC69);

ENUM(BenderCC, int8_t, None, Pitchbend, CC01, CC02, CC03, CC04, CC05, CC06, CC07, CC08, CC09, CC10, CC11, CC12, CC13,
     CC14, CC15, CC16, CC17, CC18, CC19, CC20, CC21, CC22, CC23, CC24, CC25, CC26, CC27, CC28, CC29, CC30, CC31);

ENUM(AftertouchCC, int8_t, None, ChannelPressure, CC01, CC02, CC03, CC04, CC05, CC06, CC07, CC08, CC09, CC10, CC11,
     CC12, CC13, CC14, CC15, CC16, CC17, CC18, CC19, CC20, CC21, CC22, CC23, CC24, CC25, CC26, CC27, CC28, CC29, CC30,
     CC31, PitchbendUp, PitchbendDown);

ENUM(AftertouchCurves, int8_t, AFTERTOUCH_CURVE_SOFT = 0, AFTERTOUCH_CURVE_NORMAL = 1, AFTERTOUCH_CURVE_HARD = 2)

ENUM(VelocityCurves, int8_t, VELOCITY_CURVE_VERY_SOFT = 0, VELOCITY_CURVE_SOFT = 1, VELOCITY_CURVE_NORMAL = 2,
     VELOCITY_CURVE_HARD = 3, VELOCITY_CURVE_VERY_HARD = 4)

template <typename T> inline typename std::enable_if<std::is_floating_point<T>::value, bool>::type differs(T a, T b)
{
  return std::abs(a - b) > std::numeric_limits<T>::epsilon();
}

#ifdef _DEVELOPMENT_PC
constexpr auto isDevelopmentPC = true;
#else
constexpr auto isDevelopmentPC = false;
#endif
