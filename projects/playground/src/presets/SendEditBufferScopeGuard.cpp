#include "SendEditBufferScopeGuard.h"
#include <Application.h>
#include <proxies/audio-engine/AudioEngineProxy.h>
#include <libundo/undo/Transaction.h>

SendEditBufferScopeGuard::SendEditBufferScopeGuard(UNDO::Transaction* transaction)
    : m_transaction { transaction }
{
  m_transaction->addSimpleCommand([this](auto s) {
    if(s == UNDO::Transaction::UNDOING)
      Application::get().getAudioEngineProxy()->thawParameterMessages();
    else
      Application::get().getAudioEngineProxy()->freezeParameterMessages();
  });
}

SendEditBufferScopeGuard::~SendEditBufferScopeGuard()
{
  m_transaction->addSimpleCommand([this](auto s) {
    if(s == UNDO::Transaction::REDOING || s == UNDO::Transaction::DOING)
      Application::get().getAudioEngineProxy()->thawParameterMessages();
    else
      Application::get().getAudioEngineProxy()->freezeParameterMessages();
  });
}
