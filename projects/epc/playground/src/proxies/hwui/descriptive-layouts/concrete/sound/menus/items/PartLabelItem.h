#pragma once

#include <proxies/hwui/descriptive-layouts/concrete/menu/menu-items/BasicItemWithValueText.h>

class PartLabelItem : public BasicItemWithValueText
{
 public:
  explicit PartLabelItem(const Rect& r);
  void doAction() override;
};
