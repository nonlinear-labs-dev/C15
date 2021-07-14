#pragma once

#include <proxies/hwui/Layout.h>

namespace UNDO
{
  class Transaction;
}

class UndoList;
class UndoMenu;
class Button;
class Label;
class RamUsageLabel;
class PositionInTreeLabel;

class UndoLayout : public Layout
{
 private:
  typedef Layout super;

 public:
  UndoLayout();
  ~UndoLayout() override;

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  bool onRotary(int inc, ButtonModifiers modifiers) override;

 private:
  bool isTipParentOfCurrentUndo() const;
  void doEditAction();
  void assignTransactions();
  void moveTipBy(int inc);
  void jumpToTip();
  void selectPreviousSibling();
  void selectNextSibling();
  bool canNext() const;
  bool canPrev() const;

  UNDO::Transaction *getTip() const;
  UNDO::Transaction *jumpForward(UNDO::Transaction *tip, int inc);
  UNDO::Transaction *jumpBackward(UNDO::Transaction *tip, int inc);
  void toggleEdit();

  UNDO::Transaction *m_tip = nullptr;

  UndoList *m_list = nullptr;
  Button *m_editButton = nullptr;
  Label *m_editMode = nullptr;
  PositionInTreeLabel *m_posInTree = nullptr;
};
