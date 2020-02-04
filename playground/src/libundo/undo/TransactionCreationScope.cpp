#include "TransactionCreationScope.h"
#include "Transaction.h"

namespace UNDO
{

  TransactionCreationScope::TransactionCreationScope(Transaction *transaction, bool ownTransaction)
      : m_transaction(transaction)
      , m_transactionOwned(ownTransaction)
  {
  }

  TransactionCreationScope::~TransactionCreationScope()
  {
    if(!m_transaction->isClosed())
      m_transaction->close();

    if(m_transactionOwned)
      delete m_transaction;
  }

  Transaction *TransactionCreationScope::getTransaction() const
  {
    return m_transaction;
  }

} /* namespace UNDO */
