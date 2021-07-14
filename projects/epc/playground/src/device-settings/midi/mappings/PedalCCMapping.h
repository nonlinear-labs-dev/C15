#pragma once
#include <device-settings/EnumSetting.h>
#include <nltools/Types.h>
#include <Application.h>
#include "RibbonCCMapping.h"

template <int tPedal> class PedalCCMapping : public EnumSetting<PedalCC>
{
 public:
  constexpr static auto PedalID = tPedal;

  PedalCCMapping(UpdateDocumentContributor& s)
      : EnumSetting<PedalCC>(s, getDefault(tPedal))
  {
    static_assert(PedalID > 0 && PedalID <= 4, "PedalCCMapping takes pedal template argument of 1,2,3 or 4");
  }

 private:
  static inline constexpr PedalCC getDefault(int pedal)
  {
    switch(pedal)
    {
      case 1:
        return PedalCC::CC20;
      case 2:
        return PedalCC::CC21;
      case 3:
        return PedalCC::CC22;
      case 4:
        return PedalCC::CC23;
      default:
        break;
    }
    nltools_detailedAssertAlways(pedal > 0 && pedal <= 4,
                                 "invalid template argument tPedal, out of bounds, use 1 to 4");
    return PedalCC::CC01;
  }

 public:
  const std::vector<Glib::ustring>& enumToString() const override
  {
    static auto ret = StringTools::convert(getAllStrings<PedalCC>());
    return ret;
  }

  const std::vector<Glib::ustring>& enumToDisplayString() const override
  {
    static std::vector<Glib::ustring> ret = { "None",
                                              "CC 01 (LSB: CC 33)",
                                              "CC 02 (LSB: CC 34)",
                                              "CC 03 (LSB: CC 35)",
                                              "CC 04 (LSB: CC 36)",
                                              "CC 05 (LSB: CC 37)",
                                              "CC 06 (LSB: CC 38)",
                                              "CC 07 (LSB: CC 39)",
                                              "CC 08 (LSB: CC 40)",
                                              "CC 09 (LSB: CC 41)",
                                              "CC 10 (LSB: CC 42)",
                                              "CC 11 (LSB: CC 43)",
                                              "CC 12 (LSB: CC 44)",
                                              "CC 13 (LSB: CC 45)",
                                              "CC 14 (LSB: CC 46)",
                                              "CC 15 (LSB: CC 47)",
                                              "CC 16 (LSB: CC 48)",
                                              "CC 17 (LSB: CC 49)",
                                              "CC 18 (LSB: CC 50)",
                                              "CC 19 (LSB: CC 51)",
                                              "CC 20 (LSB: CC 52)",
                                              "CC 21 (LSB: CC 53)",
                                              "CC 22 (LSB: CC 54)",
                                              "CC 23 (LSB: CC 55)",
                                              "CC 24 (LSB: CC 56)",
                                              "CC 25 (LSB: CC 57)",
                                              "CC 26 (LSB: CC 58)",
                                              "CC 27 (LSB: CC 59)",
                                              "CC 28 (LSB: CC 60)",
                                              "CC 29 (LSB: CC 61)",
                                              "CC 30 (LSB: CC 62)",
                                              "CC 31 (LSB: CC 63)",
                                              "CC 64 (switching)",
                                              "CC 65 (switching)",
                                              "CC 66 (switching)",
                                              "CC 67 (switching)",
                                              "CC 68 (switching)",
                                              "CC 69 (switching)" };
    return ret;
  }
};