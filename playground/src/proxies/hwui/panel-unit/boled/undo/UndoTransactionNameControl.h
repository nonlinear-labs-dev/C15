#pragma once

#include "UndoEntryLabel.h"

class UndoTransactionNameControl : public UndoEntryLabel
{
 private:
  typedef UndoEntryLabel super;

 public:
  UndoTransactionNameControl(const Rect &r);
  virtual ~UndoTransactionNameControl();

  virtual void assignTransaction(shared_ptr<UNDO::Transaction> transaction, bool selected, bool current) override;

 private:
  Glib::ustring shortenStringIfNeccessary(shared_ptr<Font> font, const Glib::ustring &text) const override;
};
