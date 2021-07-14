#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>
#include "UndoTransactionClient.h"

class UndoListEntry : public ControlWithChildren, public UndoTransactionClient
{
 private:
  typedef ControlWithChildren super;

 public:
  explicit UndoListEntry(const Rect &pos);
  ~UndoListEntry() override;

  bool redraw(FrameBuffer &fb) override;
  void assignTransaction(UNDO::Transaction *transaction, bool selected, bool current) override;

 private:
  bool m_selected = false;
  bool m_current = false;
};
