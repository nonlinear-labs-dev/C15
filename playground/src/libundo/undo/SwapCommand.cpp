#include "SwapCommand.h"

namespace UNDO
{

  SwapCommand::SwapCommand(tAction doUndoRedoAction)
      : m_theAction(doUndoRedoAction)
  {
  }

  SwapCommand::~SwapCommand()
  {
  }

  void SwapCommand::implDoAction() const
  {
    m_theAction(getState());
  }

  void SwapCommand::implUndoAction() const
  {
    m_theAction(getState());
  }

  void SwapCommand::implRedoAction() const
  {
    m_theAction(getState());
  }

} /* namespace UNDO */
