#include <utility>

#include <proxies/hwui/panel-unit/boled/file/RenameExportLayout.h>

RenameExportLayout::RenameExportLayout(Bank *bank, Callback callBack)
    : initialName(bank->getName(true))
    , m_bank(bank)
    , callBackAfterCommit(std::move(callBack))
{
}

void RenameExportLayout::commit(const Glib::ustring &newName)
{
  callBackAfterCommit(newName, m_bank);
}

Glib::ustring RenameExportLayout::getInitialText() const
{
  return initialName;
}
