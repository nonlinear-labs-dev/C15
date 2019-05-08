#pragma once

#include "UndoEntryLabel.h"

class PrevUndoTransactionSiblingControl : public UndoEntryLabel
{
 private:
  typedef UndoEntryLabel super;

 public:
  PrevUndoTransactionSiblingControl(const Rect &r);
  virtual ~PrevUndoTransactionSiblingControl();

  virtual void assignTransaction(shared_ptr<UNDO::Transaction> transaction, bool selected, bool current) override;

  shared_ptr<Font> getFont() const override;
  void setFontColor(FrameBuffer &fb) const override;

 private:
  bool m_bold = false;

  void setBold(bool bold);
};
