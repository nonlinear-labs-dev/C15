#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>

namespace UNDO
{
  class Transaction;
}

class UndoList : public ControlWithChildren
{
 private:
  typedef ControlWithChildren super;

 public:
  explicit UndoList(const Rect &pos);

  void assignTransactions(UNDO::Transaction *tip);
  bool redraw(FrameBuffer &fb) override;
};
