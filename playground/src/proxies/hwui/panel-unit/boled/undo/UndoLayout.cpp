#include "UndoLayout.h"
#include "UndoList.h"
#include <Application.h>
#include <http/UndoScope.h>
#include <proxies/hwui/controls/Button.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/InvertedLabel.h>
#include "UndoMenu.h"
#include "PositionInTreeLabel.h"

UndoLayout::UndoLayout()
    : super(Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled())
{
  m_list = addControl(new UndoList(Rect(0, 0, 192, 64)));
  m_editButton = addControl(new Button("", Buttons::BUTTON_D));

  m_posInTree = addControl(new PositionInTreeLabel(Rect(195, 0, 58, 11)));

  Application::get().getUndoScope()->onUndoScopeChanged(mem_fun(this, &UndoLayout::assignTransactions));
}

UndoLayout::~UndoLayout()
{
}

bool UndoLayout::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  if(down)
  {
    switch(i)
    {
      case Buttons::BUTTON_ENTER:
        jumpToTip();
        return true;

      case Buttons::BUTTON_A:
        selectPreviousSibling();
        return true;

      case Buttons::BUTTON_C:
        selectNextSibling();
        return true;

      case Buttons::BUTTON_D:
        if(m_editMode)
          doEditAction();
        return true;

      case Buttons::BUTTON_EDIT:
        toggleEdit();
        return true;

      case Buttons::BUTTON_PRESET:
        Application::get().getHWUI()->undoableSetFocusAndMode(FocusAndMode(UIFocus::Presets, UIMode::Select));
        return true;

      case Buttons::BUTTON_STORE:
        Application::get().getHWUI()->undoableSetFocusAndMode(FocusAndMode(UIFocus::Presets, UIMode::Store));
        return true;
    }
  }

  return super::onButton(i, down, modifiers);
}

void UndoLayout::doEditAction()
{
  if(m_editMode)
  {
    if(auto tip = getTip())
    {
      if(isTipParentOfCurrentUndo())
      {
        Application::get().getUndoScope()->rebase(tip);
      }
      else
      {
        moveTipBy(-1);
        Application::get().getUndoScope()->eraseBranch(tip);
      }

      assignTransactions();
    }
  }
}

bool UndoLayout::isTipParentOfCurrentUndo() const
{
  auto walker = Application::get().getUndoScope()->getUndoTransaction();
  auto tip = getTip();

  while(walker)
  {
    if(walker == tip)
      return true;

    walker = walker->getPredecessor();
  }

  return false;
}

void UndoLayout::toggleEdit()
{
  if(m_editMode)
  {
    remove(m_editMode);
    m_editMode = nullptr;
  }
  else
  {
    m_editMode = addControl(new InvertedLabel("Edit", Rect(195, 16, 58, 11)));
    m_editMode->setHighlight(true);
  }

  assignTransactions();
}

bool UndoLayout::onRotary(int inc, ButtonModifiers modifiers)
{
  moveTipBy(inc);
  return true;
}

UNDO::Transaction *UndoLayout::getTip() const
{
  return m_tip ? m_tip : Application::get().getUndoScope()->getUndoTransaction();
}

void UndoLayout::assignTransactions()
{
  auto tip = getTip();
  m_list->assignTransactions(tip);

  m_posInTree->assignTransaction(tip, false, false);

  if(m_editMode)
  {
    if(isTipParentOfCurrentUndo())
      m_editButton->setText("Make Root");
    else
      m_editButton->setText("Delete");
  }
  else
    m_editButton->setText("");
}

bool UndoLayout::canNext() const
{
  if(auto tip = getTip())
    if(auto parent = tip->getPredecessor())
      return parent->getSuccessor(parent->getNumSuccessors() - 1) != tip;

  return false;
}

bool UndoLayout::canPrev() const
{
  if(auto tip = getTip())
    if(auto parent = tip->getPredecessor())
      return parent->getSuccessor(0) != tip;

  return false;
}

void UndoLayout::moveTipBy(int inc)
{
  auto oldTip = getTip();
  auto tip = inc > 0 ? jumpForward(oldTip, inc) : jumpBackward(oldTip, inc);

  if(oldTip != tip)
  {
    m_tip = tip;
    assignTransactions();
  }
}

UNDO::Transaction *UndoLayout::jumpForward(UNDO::Transaction *tip, int inc)
{
  if(inc > 0)
    if(auto next = tip->getDefaultRedoRoute())
      return jumpForward(next, inc - 1);

  return tip;
}

UNDO::Transaction *UndoLayout::jumpBackward(UNDO::Transaction *tip, int inc)
{
  if(inc < 0)
    if(auto next = tip->getPredecessor())
      return jumpBackward(next, inc + 1);

  return tip;
}

void UndoLayout::jumpToTip()
{
  if(auto tip = getTip())
  {
    Application::get().getHWUI()->freezeFocusAndMode();
    Application::get().getUndoScope()->undoJump(tip);
    Application::get().getHWUI()->thawFocusAndMode();
  }
}

void UndoLayout::selectPreviousSibling()
{
  if(auto tip = getTip())
  {
    if(auto parent = tip->getPredecessor())
    {
      size_t numSuccessors = parent->getNumSuccessors();
      UNDO::Transaction *before = nullptr;

      for(size_t i = 0; i < numSuccessors; i++)
      {
        auto currentSuccessor = parent->getSuccessor(i);

        if(currentSuccessor == tip)
        {
          if(before)
          {
            m_tip = before;
            assignTransactions();
          }
          return;
        }
        before = currentSuccessor;
      }
    }
  }
}

void UndoLayout::selectNextSibling()
{
  if(auto tip = getTip())
  {
    if(auto parent = tip->getPredecessor())
    {
      size_t numSuccessors = parent->getNumSuccessors();
      bool foundIt = false;

      for(size_t i = 0; i < numSuccessors; i++)
      {
        auto currentSuccessor = parent->getSuccessor(i);

        if(foundIt)
        {
          m_tip = currentSuccessor;
          assignTransactions();
          return;
        }

        if(currentSuccessor == tip)
        {
          foundIt = true;
        }
      }
    }
  }
}
