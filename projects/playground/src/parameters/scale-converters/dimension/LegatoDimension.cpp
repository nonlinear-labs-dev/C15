#include "LegatoDimension.h"

Glib::ustring LegatoDimension::stringize(const tDisplayValue& displayValue) const
{
  switch(static_cast<int>(displayValue))
  {
    default:
    case 0:
      return "None";
    case 1:
      return "Envelopes";
    case 2:
      return "Glide";
    case 3:
      return "Env & Glide";
  }
}

const LegatoDimension& LegatoDimension::get()
{
  static LegatoDimension ret;
  return ret;
}

Glib::ustring LegatoDimension::getStingizerJS() const
{
  return "stringizeLegato(cpValue)";
}
