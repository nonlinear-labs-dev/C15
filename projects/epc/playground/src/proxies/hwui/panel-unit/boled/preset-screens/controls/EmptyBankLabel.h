#pragma once

#include "proxies/hwui/controls/Label.h"

class EmptyBankLabel : public Label
{
 private:
  typedef Label super;

 public:
  explicit EmptyBankLabel(const Rect &pos);
  ~EmptyBankLabel() override;

  EmptyBankLabel(const EmptyBankLabel &other) = delete;
  EmptyBankLabel &operator=(const EmptyBankLabel &) = delete;

  bool redraw(FrameBuffer &fb) override;
  void setFontColor(FrameBuffer &fb) const override;
};
