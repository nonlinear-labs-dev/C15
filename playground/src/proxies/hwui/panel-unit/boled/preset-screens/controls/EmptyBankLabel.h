#pragma once

#include "proxies/hwui/controls/Label.h"

class EmptyBankLabel : public Label
{
 private:
  typedef Label super;

 public:
  // public methods
  EmptyBankLabel(const Rect &pos);
  virtual ~EmptyBankLabel();

  bool redraw(FrameBuffer &fb);
  void setFontColor(FrameBuffer &fb) const;

 private:
  // private methods
  EmptyBankLabel(const EmptyBankLabel &other);
  EmptyBankLabel &operator=(const EmptyBankLabel &);
};
