#pragma once

#include "proxies/hwui/controls/Label.h"

class Application;

class BankAndPresetNumberLabel : public Label
{
 private:
  typedef Label super;

 public:
  BankAndPresetNumberLabel(const Rect& pos);
  virtual bool redraw(FrameBuffer& fb) override;

 private:
  void onEditBufferChanged();
};
