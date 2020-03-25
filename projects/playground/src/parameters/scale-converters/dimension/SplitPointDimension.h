#pragma once

#include <nltools/Types.h>
#include "KeyDimension.h"

class SplitPointDimension : public KeyDimension
{
 public:
  static Glib::ustring stringizeSplitPointDisplay(tControlPositionValue value, VoiceGroup seenFrom);
  Glib::ustring stringize(const tDisplayValue &displayValue) const override;
  static const SplitPointDimension &get();
};
