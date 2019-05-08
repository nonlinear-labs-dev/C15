#include <proxies/hwui/panel-unit/UndoButtonsStateMachine.h>
#include <Application.h>
#include <http/UndoScope.h>
#include <proxies/hwui/HWUI.h>

UndoButtonStateMachine::UndoButtonStateMachine()
{
  registerTraversal(UNDO_STATE_INITIAL, UNDO_PRESSED, UNDO_STATE_WAIT_FOR_UNDO_OR_SCREEN);
  registerTraversal(UNDO_STATE_WAIT_FOR_UNDO_OR_SCREEN, UNDO_RELEASED, UNDO_STATE_UNDO);
  registerTraversal(UNDO_STATE_WAIT_FOR_UNDO_OR_SCREEN, REDO_PRESSED, UNDO_STATE_SCREEN);

  registerTraversal(UNDO_STATE_INITIAL, REDO_PRESSED, UNDO_STATE_WAIT_FOR_REDO_OR_SCREEN);
  registerTraversal(UNDO_STATE_WAIT_FOR_REDO_OR_SCREEN, REDO_RELEASED, UNDO_STATE_REDO);
  registerTraversal(UNDO_STATE_WAIT_FOR_REDO_OR_SCREEN, UNDO_PRESSED, UNDO_STATE_SCREEN);
}

UndoButtonStateMachine::~UndoButtonStateMachine()
{
}

bool UndoButtonStateMachine::setState(UndoStates s)
{
  switch(s)
  {
    case UNDO_STATE_UNDO:
      Application::get().getUndoScope()->undo();
      s = UNDO_STATE_INITIAL;
      break;

    case UNDO_STATE_REDO:
      Application::get().getUndoScope()->redo();
      s = UNDO_STATE_INITIAL;
      break;

    case UNDO_STATE_SCREEN:
      Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().showUndoScreen();
      s = UNDO_STATE_INITIAL;
      break;

    case UNDO_STATE_INVALID:
      s = UNDO_STATE_INITIAL;
      break;
  }

  return super::setState(s);
}
