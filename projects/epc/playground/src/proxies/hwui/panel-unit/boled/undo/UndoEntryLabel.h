#pragma once

#include <proxies/hwui/controls/Label.h>
#include "UndoTransactionClient.h"

class UndoEntryLabel : public Label, public UndoTransactionClient
{
 private:
  typedef Label super;

 public:
  explicit UndoEntryLabel(const Rect &pos);
  ~UndoEntryLabel() override;

  void assignTransaction(UNDO::Transaction *transaction, bool selected, bool current) override;
  void setBackgroundColor(FrameBuffer &fb) const override;
};
