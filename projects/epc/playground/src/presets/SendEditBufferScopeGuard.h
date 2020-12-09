#pragma once

namespace UNDO
{
  class Transaction;
}

class SendEditBufferScopeGuard
{
 public:
  SendEditBufferScopeGuard(UNDO::Transaction* transaction, bool sendToAE);
  ~SendEditBufferScopeGuard();

 private:
  const bool sendToAudioEngine;
  UNDO::Transaction* m_transaction;
};
