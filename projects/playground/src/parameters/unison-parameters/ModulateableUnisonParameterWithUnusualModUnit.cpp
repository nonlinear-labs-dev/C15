#include "ModulateableUnisonParameterWithUnusualModUnit.h"

Layout *ModulateableUnisonParameterWithUnusualModUnit::createLayout(FocusAndMode focusAndMode) const
{
  switch(focusAndMode.mode)
  {
    case UIMode::Select:
    default:
      return new ModulateableUnisonParameterLayout();
    case UIMode::Edit:
      return new ModulateableParameterEditLayout2();
    case UIMode::Info:
      return new ParameterInfoLayout();
  }
}
