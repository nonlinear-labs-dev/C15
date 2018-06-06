#pragma once

#include <proxies/hwui/DFBLayout.h>

namespace UNDO
{
  class Transaction;
}

class UndoList;
class UndoMenu;
class Button;
class Label;
class PositionInTreeLabel;

class UndoLayout : public DFBLayout
{
  private:
    typedef DFBLayout super;

  public:
    UndoLayout ();
    virtual ~UndoLayout ();

    virtual bool onButton (Buttons i, bool down, ButtonModifiers modifiers) override;
    virtual bool onRotary (int inc, ButtonModifiers modifiers) override;

  private:
    bool isTipParentOfCurrentUndo () const;
    void doEditAction ();
    void assignTransactions ();
    void moveTipBy (int inc);
    void jumpToTip ();
    void selectPreviousSibling ();
    void selectNextSibling ();
    bool canNext () const;
    bool canPrev () const;

    shared_ptr<UNDO::Transaction> getTip () const;
    shared_ptr<UNDO::Transaction> jumpForward (shared_ptr<UNDO::Transaction> tip, int inc);
    shared_ptr<UNDO::Transaction> jumpBackward (shared_ptr<UNDO::Transaction> tip, int inc);
    void toggleEdit ();

    shared_ptr<UNDO::Transaction> m_tip;

    UndoList *m_list = nullptr;
    Button *m_editButton = nullptr;
    Label *m_editMode = nullptr;
    PositionInTreeLabel *m_posInTree = nullptr;
};

