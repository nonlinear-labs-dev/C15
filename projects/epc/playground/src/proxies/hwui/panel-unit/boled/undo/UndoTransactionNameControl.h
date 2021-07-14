#pragma once

#include "UndoEntryLabel.h"

class UndoTransactionNameControl : public UndoEntryLabel
{
 private:
  typedef UndoEntryLabel super;

 public:
  explicit UndoTransactionNameControl(const Rect &r);
  ~UndoTransactionNameControl() override;

  void assignTransaction(UNDO::Transaction *transaction, bool selected, bool current) override;

 private:
  StringAndSuffix shortenStringIfNeccessary(const std::shared_ptr<Font> &font,
                                            const StringAndSuffix &text) const override;
};
