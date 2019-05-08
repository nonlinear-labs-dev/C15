#pragma once

#include "RenameLayout.h"
#include <libundo/undo/Transaction.h>

class PresetBank;

class EditBankInfoLayout : public RenameLayout
{
 private:
  typedef RenameLayout super;

 public:
  EditBankInfoLayout();

 protected:
  virtual void commit(const Glib::ustring &newName) override;
  virtual Glib::ustring getInitialText() const override;

 private:
  shared_ptr<PresetBank> m_currentBank;
};
