#pragma once
#include <nltools/Types.h>
#include <device-settings/EnumSetting.h>
#include <Application.h>
#include <tools/StringTools.h>

template <int numRibbon> class RibbonCCMapping : public EnumSetting<RibbonCC>
{
 public:
  constexpr static auto RibbonID = numRibbon;

  RibbonCCMapping(UpdateDocumentContributor& u)
      : EnumSetting<RibbonCC>(u, getDefault(RibbonID))
  {
    static_assert(RibbonID > 0 && RibbonID <= 2, "Ribbon ID should be either 1 or 2");
  }

  static constexpr RibbonCC getDefault(int id)
  {
    if(id == 1)
      return RibbonCC::CC24;
    else if(id == 2)
      return RibbonCC::CC25;

    nltools_assertNotReached();
  }
  const std::vector<Glib::ustring>& enumToString() const override;
  const std::vector<Glib::ustring>& enumToDisplayString() const override;
};

template <int numRibbon> const std::vector<Glib::ustring>& RibbonCCMapping<numRibbon>::enumToString() const
{
  static std::vector<Glib::ustring> ret = StringTools::convert(getAllStrings<RibbonCC>());
  return ret;
}

template <int numRibbon> const std::vector<Glib::ustring>& RibbonCCMapping<numRibbon>::enumToDisplayString() const
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
                                            "CC 31 (LSB: CC 63)" };
  return ret;
}
