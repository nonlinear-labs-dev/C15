#pragma once

#include <parameters/Parameter.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/UnisonParameterLayouts.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/UnmodulatebaleParameterLayouts.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterInfoLayout.h>

class UnmodulateableUnisonParameter : public Parameter
{
 public:
  template <typename... tArgs>
  explicit UnmodulateableUnisonParameter(tArgs... args)
      : Parameter(args...)
  {
  }

  Layout *createLayout(FocusAndMode focusAndMode) const override;
};
