#pragma once
#include "Dimension.h"

class OnOffDimension : public Dimension
{
 public:
  Glib::ustring stringize(const tDisplayValue& displayValue) const override;
  Glib::ustring getStingizerJS() const override;

  static OnOffDimension& get()
  {
    static OnOffDimension ret;
    return ret;
  }
};
