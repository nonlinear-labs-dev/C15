#pragma once

#include "RenameLayout.h"
#include <libundo/undo/Scope.h>

class PresetBank;

class RenameBankLayout : public RenameLayout
{
 private:
  typedef RenameLayout super;

 public:
  RenameBankLayout(UNDO::Scope::tTransactionPtr transaction = nullptr);

 private:
  virtual void commit(const Glib::ustring &newName) override;
  virtual Glib::ustring getInitialText() const override;

  shared_ptr<PresetBank> m_currentBank;
  UNDO::Scope::tTransactionPtr m_transaction;
};
