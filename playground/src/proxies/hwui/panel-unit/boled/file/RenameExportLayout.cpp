#include <proxies/hwui/panel-unit/boled/file/RenameExportLayout.h>

RenameExportLayout::RenameExportLayout(std::shared_ptr<PresetBank> bank,
                                       std::function<void(Glib::ustring, std::shared_ptr<PresetBank>)> callBack)
    : initialName(bank->getName(true))
    , callBackAfterCommit(callBack)
    , m_bank(bank)
{
}

RenameExportLayout::~RenameExportLayout()
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
