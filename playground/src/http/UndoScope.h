#pragma once

#include "libundo/undo/Scope.h"

class UndoScope : public UNDO::Scope
{
 private:
  typedef UNDO::Scope super;

 public:
  using UNDO::Scope::Scope;

  virtual tUpdateID onChange(uint64_t flags = UpdateDocumentContributor::ChangeFlags::Generic) override;
  sigc::connection onUndoScopeChanged(slot<void> cb);

 protected:
  virtual void onAddTransaction(tTransactionPtr transaction);

 private:
  Signal<void> m_sigUndoScopeChanged;
};
