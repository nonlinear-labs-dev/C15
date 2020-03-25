#include "AftertouchParameter.h"
#include <proxies/hwui/panel-unit/boled/parameter-screens/PlayControlParameterLayouts.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterInfoLayout.h>
#include <glib.h>

ReturnMode AftertouchParameter::getReturnMode() const
{
  return ReturnMode::Zero;
}

Layout *AftertouchParameter::createLayout(FocusAndMode focusAndMode) const
{
  switch(focusAndMode.mode)
  {
    case UIMode::Info:
      return new ParameterInfoLayout();

    case UIMode::Edit:
      return new AftertouchParameterEditLayout2();

    case UIMode::Select:
    default:
      return new AftertouchParameterSelectLayout2();
  }

  g_return_val_if_reached(nullptr);
}
