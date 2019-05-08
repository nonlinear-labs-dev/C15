#include "TransactionCreationScope.h"

namespace UNDO
{

  TransactionCreationScope::TransactionCreationScope(tTransactionPtr transaction)
      : m_transaction(transaction)
  {
  }

  TransactionCreationScope::~TransactionCreationScope()
  {
    m_transaction->close();
  }

  TransactionCreationScope::tTransactionPtr TransactionCreationScope::getTransaction()
  {
    return m_transaction;
  }

} /* namespace UNDO */
