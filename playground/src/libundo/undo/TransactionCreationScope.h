#pragma once

#include "Transaction.h"

namespace UNDO
{

  class TransactionCreationScope
  {
   public:
    typedef std::shared_ptr<Transaction> tTransactionPtr;
    typedef std::shared_ptr<TransactionCreationScope> tTransactionScopePtr;
    TransactionCreationScope(tTransactionPtr transaction);
    virtual ~TransactionCreationScope();

    tTransactionPtr getTransaction();

   private:
    tTransactionPtr m_transaction;
  };

} /* namespace UNDO */