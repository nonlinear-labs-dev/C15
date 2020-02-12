#include "OnOffDimension.h"

Glib::ustring OnOffDimension::stringize(const tDisplayValue& displayValue) const
{
  return displayValue == 0 ? "Off" : "On";
}

Glib::ustring OnOffDimension::getStingizerJS() const
{
  return "formatOnOff(cpValue)";
}
