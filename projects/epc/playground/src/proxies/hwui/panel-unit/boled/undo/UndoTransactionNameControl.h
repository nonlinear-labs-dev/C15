#pragma once

#include "UndoEntryLabel.h"

class UndoTransactionNameControl : public UndoEntryLabel
{
 private:
  typedef UndoEntryLabel super;

 public:
  UndoTransactionNameControl(const Rect &r);
  virtual ~UndoTransactionNameControl();

  virtual void assignTransaction(UNDO::Transaction *transaction, bool selected, bool current) override;

 private:
  StringAndSuffix shortenStringIfNeccessary(std::shared_ptr<Font> font, const StringAndSuffix &text) const override;
};
