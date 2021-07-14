#pragma once

#include "proxies/hwui/controls/Label.h"

class Application;

class BankAndPresetNumberLabel : public Label
{
 private:
  typedef Label super;

 public:
  explicit BankAndPresetNumberLabel(const Rect& pos);
  bool redraw(FrameBuffer& fb) override;

 private:
  void onEditBufferChanged();
};
