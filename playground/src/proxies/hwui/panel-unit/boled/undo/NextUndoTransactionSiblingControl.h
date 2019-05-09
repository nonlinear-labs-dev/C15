#pragma once

#include "UndoEntryLabel.h"

class NextUndoTransactionSiblingControl : public UndoEntryLabel
{
 private:
  typedef UndoEntryLabel super;

 public:
  NextUndoTransactionSiblingControl(const Rect &r);
  virtual ~NextUndoTransactionSiblingControl();

<<<<<<< HEAD
  virtual void assignTransaction(UNDO::Transaction *transaction, bool selected, bool current) override;

  std::shared_ptr<Font> getFont() const override;
=======
  virtual void assignTransaction(shared_ptr<UNDO::Transaction> transaction, bool selected, bool current) override;

  shared_ptr<Font> getFont() const override;
>>>>>>> layouts-reloaded
  void setFontColor(FrameBuffer &fb) const override;

 private:
  bool m_bold = false;

  void setBold(bool bold);
};
