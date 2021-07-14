#pragma once

#include "Dimension.h"

class VoicesDimension : public Dimension
{
 private:
  VoicesDimension();

 public:
  virtual ~VoicesDimension();

  static const VoicesDimension &get();
  [[nodiscard]] Glib::ustring getStingizerJS() const override;
  [[nodiscard]] Glib::ustring stringize(const tDisplayValue &displayValue) const override;
};
