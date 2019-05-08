#include "ActionCommand.h"

namespace UNDO
{

  ActionCommand::ActionCommand(tAction doAction, tAction undoAction, tAction redoAction)
      : m_doAction(doAction)
      , m_undoAction(undoAction)
      , m_redoAction(redoAction)
  {
  }

  ActionCommand::ActionCommand(tAction doRedoAction, tAction undoAction)
      : m_doAction(doRedoAction)
      , m_undoAction(undoAction)
      , m_redoAction(doRedoAction)
  {
  }

  ActionCommand::ActionCommand(tAction doRedoUndoAction)
      : m_doAction(doRedoUndoAction)
      , m_undoAction(doRedoUndoAction)
      , m_redoAction(doRedoUndoAction)
  {
  }

  ActionCommand::~ActionCommand()
  {
  }

  void ActionCommand::implDoAction() const
  {
    if(m_doAction)
      m_doAction(getState());
  }

  void ActionCommand::implUndoAction() const
  {
    if(m_undoAction)
      m_undoAction(getState());
  }

  void ActionCommand::implRedoAction() const
  {
    if(m_redoAction)
      m_redoAction(getState());
  }

} /* namespace UNDO */
