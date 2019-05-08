#pragma once

#include "Dimension.h"

class VoicesDimension : public Dimension
{
 private:
  VoicesDimension();

 public:
  virtual ~VoicesDimension();

  static const VoicesDimension &get();
  Glib::ustring getStingizerJS() const override;
  Glib::ustring stringize(const tDisplayValue &displayValue) const override;
};
