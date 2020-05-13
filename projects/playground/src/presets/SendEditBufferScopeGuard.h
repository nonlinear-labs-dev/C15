#pragma once

#include <libundo/undo/Transaction.h>
class EditBuffer;
class SendEditBufferScopeGuard
{
 public:
  SendEditBufferScopeGuard(EditBuffer* eb, UNDO::Transaction* transaction);
  ~SendEditBufferScopeGuard();

 private:
  UNDO::Transaction* m_transaction;
  EditBuffer* m_editBuffer;
};