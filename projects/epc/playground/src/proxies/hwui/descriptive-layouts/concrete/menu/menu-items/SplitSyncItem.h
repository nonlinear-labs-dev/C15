#pragma once
#include "BasicItemWithValueText.h"

class Setting;

class SplitSyncItem : public BasicItemWithValueText
{
 public:
  explicit SplitSyncItem(const Rect& r);
  void doAction() override;
  bool redraw(FrameBuffer& fb) override;

 private:
  void onSettingChanged(const Setting* s);
};