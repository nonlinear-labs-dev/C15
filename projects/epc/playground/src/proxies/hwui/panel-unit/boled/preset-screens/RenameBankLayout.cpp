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
{
  m_currentBank = Application::get().getPresetManager()->getSelectedBank();
}

RenameBankLayout::RenameBankLayout()
    : super()
    , m_transaction { nullptr }
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
      BankUseCases useCase(m_currentBank);
      useCase.renameBank(newName);
    }
  }
}

Glib::ustring RenameBankLayout::getInitialText() const
{
  if(m_currentBank)
    return m_currentBank->getName(false);

  return "";
}
