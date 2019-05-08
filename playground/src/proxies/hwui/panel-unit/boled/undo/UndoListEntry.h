#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>
#include "UndoTransactionClient.h"

class UndoListEntry : public ControlWithChildren, public UndoTransactionClient
{
 private:
  typedef ControlWithChildren super;

 public:
  UndoListEntry(const Rect &pos);
  virtual ~UndoListEntry();

  virtual bool redraw(FrameBuffer &fb) override;
  virtual void assignTransaction(shared_ptr<UNDO::Transaction> transaction, bool selected, bool current) override;

 private:
  bool m_selected = false;
  bool m_current = false;
};
