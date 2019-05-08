#pragma once

#include "Command.h"
#include <functional>

namespace UNDO
{

  class ActionCommand : public UNDO::Command
  {
   public:
    typedef std::function<void(State)> tAction;

    ActionCommand(tAction doAction, tAction undoAction, tAction redoAction);
    ActionCommand(tAction doRedoAction, tAction undoAction);
    ActionCommand(tAction doRedoUndoAction);
    virtual ~ActionCommand();

   protected:
    void implDoAction() const;
    void implUndoAction() const;
    void implRedoAction() const;

   private:
    tAction m_doAction;
    tAction m_undoAction;
    tAction m_redoAction;
  };

} /* namespace UNDO */