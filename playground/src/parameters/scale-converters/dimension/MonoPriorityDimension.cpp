#include "MonoPriorityDimension.h"

Glib::ustring MonoPriorityDimension::stringize(const tDisplayValue& displayValue) const
{
  auto castedDisplay = static_cast<int>(displayValue);

  switch(castedDisplay)
  {
    case 0:
      return "Lowest";
    case 1:
    default:
      return "Latest";
    case 2:
      return "Highest";
  };
}

Glib::ustring MonoPriorityDimension::getStingizerJS() const
{
  return "formatPriority(cpValue)";
}

const MonoPriorityDimension& MonoPriorityDimension::get()
{
  static MonoPriorityDimension d;
  return d;
}
