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
  UndoMenu(UNDO::Transaction *tip, const Rect &r);
  ~UndoMenu() override;

  void assignTransaction(UNDO::Transaction *tip);

 private:
  bool isParentOfCurrentUndo();

  UNDO::Transaction *m_tip = nullptr;
};
