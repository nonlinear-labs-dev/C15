#pragma once
#include "playground.h"

class Dimension
{
 public:
  Dimension();

  [[nodiscard]] virtual Glib::ustring getStingizerJS() const;
  [[nodiscard]] virtual Glib::ustring stringize(const tDisplayValue &displayValue) const = 0;

  [[nodiscard]] size_t hash() const;

 protected:
  static Glib::ustring formatDimensionRounded(tControlPositionValue in, const Glib::ustring &unit,
                                              tControlPositionValue roundTo);
  static Glib::ustring formatDimensionDigits(tControlPositionValue in, const Glib::ustring &unit, int numDigits);

 private:
  static Glib::ustring formatNegativeDimensionDigits(tControlPositionValue in, const Glib::ustring &unit,
                                                     int numDigits);
  static Glib::ustring formatPositiveDimensionDigits(tControlPositionValue in, const Glib::ustring &unit,
                                                     int numDigits);

  friend class TestableDimension;
};
