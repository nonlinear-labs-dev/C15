#include "MonoPriorityDimension.h"

Glib::ustring MonoPriorityDimension::stringize(const tDisplayValue& displayValue) const
{
  return Glib::ustring();
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
