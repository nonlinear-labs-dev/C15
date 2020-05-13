#include "SendEditBufferScopeGuard.h"
#include "presets/EditBuffer.h"

SendEditBufferScopeGuard::SendEditBufferScopeGuard(EditBuffer* eb, UNDO::Transaction* transaction)
    : m_editBuffer { eb }
    , m_transaction { transaction }
{
  m_transaction->addSimpleCommand([this](auto s) {
    if(s == UNDO::Transaction::UNDOING)
      m_editBuffer->sendToAudioEngine();
  });
}

SendEditBufferScopeGuard::~SendEditBufferScopeGuard()
{
  m_transaction->addSimpleCommand([this](auto s) {
    if(s == UNDO::Transaction::REDOING || s == UNDO::Transaction::DOING)
      m_editBuffer->sendToAudioEngine();
  });
}
