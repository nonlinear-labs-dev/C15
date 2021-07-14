#pragma once

#include "LeftAlignedLabel.h"

class CenterAlignedLabel : public LeftAlignedLabel
{
 public:
  CenterAlignedLabel(const Glib::ustring& text, const Rect& r);
  Font::Justification getJustification() const override;
};
