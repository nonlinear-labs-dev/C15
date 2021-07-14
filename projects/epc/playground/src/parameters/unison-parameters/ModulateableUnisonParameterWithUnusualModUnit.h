#pragma once

#include <parameters/ModulateableParameterWithUnusualModUnit.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/UnisonParameterLayouts.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ModulateableParameterLayouts.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterInfoLayout.h>

class ModulateableUnisonParameterWithUnusualModUnit : public ModulateableParameterWithUnusualModUnit
{
 public:
  template <typename... tArgs>
  explicit ModulateableUnisonParameterWithUnusualModUnit(tArgs... args)
      : ModulateableParameterWithUnusualModUnit(args...)
  {
  }

  Layout *createLayout(FocusAndMode focusAndMode) const override;
};