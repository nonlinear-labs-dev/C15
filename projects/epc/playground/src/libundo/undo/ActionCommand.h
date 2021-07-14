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
    explicit ActionCommand(tAction doRedoUndoAction);
    ~ActionCommand() override;

   protected:
    void implDoAction() const override;
    void implUndoAction() const override;
    void implRedoAction() const override;

   private:
    tAction m_doAction;
    tAction m_undoAction;
    tAction m_redoAction;
  };

} /* namespace UNDO */