#pragma once

#include "Transaction.h"

namespace UNDO
{

  class Algorithm
  {
   public:
    Algorithm();
    virtual ~Algorithm();

    typedef std::shared_ptr<Transaction> tTransactionPtr;

    static tTransactionPtr getCommonRoot(tTransactionPtr one, tTransactionPtr other);
    static void traverse(tTransactionPtr from, tTransactionPtr to);
    static std::list<tTransactionPtr> getPathAsList(tTransactionPtr target, const Transaction *stop);
    static tTransactionPtr find(tTransactionPtr root, const Glib::ustring &targetID);
    static tTransactionPtr find(tTransactionPtr root, const void *targetID);
  };

} /* namespace UNDO */