#pragma once

#include "Command.h"
#include <functional>

using namespace std;

namespace UNDO
{

  class SwapCommand : public UNDO::Command
  {
   public:
    typedef std::function<void(State)> tAction;

    SwapCommand(tAction doUndoRedoAction);
    virtual ~SwapCommand();

   protected:
    void implDoAction() const;
    void implUndoAction() const;
    void implRedoAction() const;

   private:
    tAction m_theAction;
  };

} /* namespace UNDO */
