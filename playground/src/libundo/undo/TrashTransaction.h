#pragma once

#include "Transaction.h"

namespace UNDO
{
  class TrashTransaction : public Transaction
  {
   public:
    TrashTransaction();
    virtual ~TrashTransaction();

    virtual void addCommand(tCommandPtr cmd) override;
  };
}
