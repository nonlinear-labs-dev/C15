#pragma once

#include <proxies/hwui/controls/ButtonMenu.h>

namespace UNDO
{
  class Transaction;
}

class UndoMenu : public ButtonMenu
{
 private:
  typedef ButtonMenu super;

 public:
  UndoMenu(shared_ptr<UNDO::Transaction> tip, const Rect &r);
  virtual ~UndoMenu();

  void assignTransaction(shared_ptr<UNDO::Transaction> tip);

 private:
  bool isParentOfCurrentUndo();

  shared_ptr<UNDO::Transaction> m_tip;
};
