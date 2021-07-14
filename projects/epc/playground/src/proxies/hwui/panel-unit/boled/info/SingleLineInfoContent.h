#pragma once

#include <proxies/hwui/controls/LeftAlignedLabel.h>

class SingleLineInfoContent : public LeftAlignedLabel
{
 public:
  explicit SingleLineInfoContent(Glib::ustring name);
  SingleLineInfoContent();
};
