#pragma once

#include <proxies/hwui/controls/Label.h>
#include "UndoTransactionClient.h"

class PositionInTreeLabel : public Label, public UndoTransactionClient
{
 private:
  typedef Label super;

 public:
  PositionInTreeLabel(const Rect &pos);
  virtual ~PositionInTreeLabel();

  virtual void assignTransaction(shared_ptr<UNDO::Transaction> transaction, bool selected, bool current) override;
};
