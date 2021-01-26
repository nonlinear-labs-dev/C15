#include "RenameBankLayout.h"
#include "controls/PresetNameLabel.h"
#include "Application.h"
#include <proxies/hwui/controls/Label.h>
#include <proxies/hwui/controls/Button.h>
#include <presets/PresetManager.h>
#include <presets/Bank.h>
#include <presets/Preset.h>
#include <libundo/undo/Transaction.h>

RenameBankLayout::RenameBankLayout(UNDO::Transaction *transaction)
    : super()
    , m_transaction(transaction)
    , m_onCommitCallback { nullptr }
{
  m_currentBank = Application::get().getPresetManager()->getSelectedBank();
}

RenameBankLayout::RenameBankLayout(UNDO::Transaction *transaction, std::function<void(UNDO::Transaction *)> onCommitCB)
    : super()
    , m_transaction { transaction }
    , m_onCommitCallback { std::move(onCommitCB) }
{
  m_currentBank = Application::get().getPresetManager()->getSelectedBank();
}

void RenameBankLayout::commit(const Glib::ustring &newName)
{
  if(m_currentBank)
  {
    if(m_transaction)
    {
      m_transaction->reopen();
      m_currentBank->setName(m_transaction, newName);
      m_transaction->close();
    }
    else
    {
      auto s = Application::get().getPresetManager()->getUndoScope().startTransaction("Rename Bank");
      m_currentBank->setName(s->getTransaction(), newName);
    }
  }

  if(m_onCommitCallback)
  {
    if(m_transaction && !m_transaction->isClosed())
      m_onCommitCallback(m_transaction);
    else
    {
      auto trash = UNDO::Scope::startTrashTransaction();
      m_onCommitCallback(trash->getTransaction());
    }
  }
}

Glib::ustring RenameBankLayout::getInitialText() const
{
  if(m_currentBank)
    return m_currentBank->getName(false);

  return "";
}
