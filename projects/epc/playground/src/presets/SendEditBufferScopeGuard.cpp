#include "SendEditBufferScopeGuard.h"
#include <Application.h>
#include <proxies/audio-engine/AudioEngineProxy.h>
#include <libundo/undo/Transaction.h>

SendEditBufferScopeGuard::SendEditBufferScopeGuard(UNDO::Transaction* transaction, bool sendToAE)
    : m_transaction { transaction }
    , sendToAudioEngine { sendToAE }
{
  m_transaction->addSimpleCommand([=](auto s) {
    if(s == UNDO::Transaction::UNDOING)
      Application::get().getAudioEngineProxy()->thawParameterMessages(sendToAudioEngine);
    else
      Application::get().getAudioEngineProxy()->freezeParameterMessages();
  });
}

SendEditBufferScopeGuard::~SendEditBufferScopeGuard()
{
  m_transaction->addSimpleCommand([=](auto s) {
    if(s == UNDO::Transaction::REDOING || s == UNDO::Transaction::DOING)
      Application::get().getAudioEngineProxy()->thawParameterMessages(sendToAudioEngine);
    else
      Application::get().getAudioEngineProxy()->freezeParameterMessages();
  });
}