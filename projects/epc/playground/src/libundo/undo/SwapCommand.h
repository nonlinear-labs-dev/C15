#pragma once

#include "Command.h"
#include <functional>

namespace UNDO
{

  class SwapCommand : public UNDO::Command
  {
   public:
    typedef std::function<void(State)> tAction;

    explicit SwapCommand(tAction doUndoRedoAction);
    ~SwapCommand() override;

   protected:
    void implDoAction() const override;
    void implUndoAction() const override;
    void implRedoAction() const override;

   private:
    tAction m_theAction;
  };

} /* namespace UNDO */
