#include <proxies/hwui/panel-unit/boled/undo/UndoMenu.h>
#include <Application.h>
#include <http/UndoScope.h>

UndoMenu::UndoMenu(shared_ptr<UNDO::Transaction> tip, const Rect &r)
    : super(r, 2)
{
  assignTransaction(tip);

  addButton("Delete", [=]() {
    if(!isParentOfCurrentUndo())
    {
      Application::get().getUndoScope()->eraseBranch(m_tip);
    }
  });

  addButton("Make Root", [=]() {
    if(isParentOfCurrentUndo())
    {
      Application::get().getUndoScope()->rebase(m_tip);
    }
  });
}

UndoMenu::~UndoMenu()
{
}

void UndoMenu::assignTransaction(shared_ptr<UNDO::Transaction> tip)
{
  m_tip = tip;
}

bool UndoMenu::isParentOfCurrentUndo()
{
  auto walker = Application::get().getUndoScope()->getUndoTransaction();

  while(walker)
  {
    if(walker == m_tip)
      return true;

    walker = walker->getPredecessor();
  }

  return false;
}
