#include <Application.h>
#include <http/UndoScope.h>
#include <presets/Preset.h>
#include <presets/PresetBank.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/EditBankInfoLayout.h>

EditBankInfoLayout::EditBankInfoLayout()
    : super()
{
  if(auto bank = Application::get().getPresetManager()->getSelectedBank())
  {
    m_currentBank = bank;
  }
}

void EditBankInfoLayout::commit(const Glib::ustring &comment)
{
  if(m_currentBank)
  {
    UNDO::Scope::tTransactionScopePtr scope = m_currentBank->getUndoScope().startTransaction("Set Bank Comment");
    m_currentBank->undoableSetAttribute(scope->getTransaction(), "Comment", comment);
  }
}

Glib::ustring EditBankInfoLayout::getInitialText() const
{
  if(m_currentBank)
    return m_currentBank->getAttribute("Comment", "");

  return "";
}
