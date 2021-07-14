#pragma once

namespace UNDO
{
  class Transaction;

  class TransactionCreationScope
  {
   public:
    explicit TransactionCreationScope(Transaction* transaction, bool ownTransaction = false);
    virtual ~TransactionCreationScope();

    [[nodiscard]] Transaction* getTransaction() const;

   private:
    Transaction* m_transaction = nullptr;
    bool m_transactionOwned = false;
  };

} /* namespace UNDO */
