#pragma once

namespace UNDO
{
  class Transaction;
}

class SendEditBufferScopeGuard
{
 public:
  SendEditBufferScopeGuard(UNDO::Transaction* transaction);
  ~SendEditBufferScopeGuard();

 private:
  UNDO::Transaction* m_transaction;
};
