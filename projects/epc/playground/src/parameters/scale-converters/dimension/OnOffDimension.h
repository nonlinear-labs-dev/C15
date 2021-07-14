#pragma once
#include "Dimension.h"

class OnOffDimension : public Dimension
{
 public:
  [[nodiscard]] Glib::ustring stringize(const tDisplayValue& displayValue) const override;
  [[nodiscard]] Glib::ustring getStingizerJS() const override;

  static OnOffDimension& get()
  {
    static OnOffDimension ret;
    return ret;
  }
};
