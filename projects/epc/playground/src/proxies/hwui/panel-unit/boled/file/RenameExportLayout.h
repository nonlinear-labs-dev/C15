#pragma once

#include <proxies/hwui/panel-unit/boled/preset-screens/RenameLayout.h>

class Bank;

class RenameExportLayout : public RenameLayout
{
 public:
  using Callback = std::function<void(Glib::ustring, Bank *)>;
  RenameExportLayout(Bank *bank, Callback callBack);

  Glib::ustring getInitialText() const override;
  void commit(const Glib::ustring &newName) override;

 private:
  Glib::ustring initialName;
  Bank *m_bank;
  Callback callBackAfterCommit;
};
