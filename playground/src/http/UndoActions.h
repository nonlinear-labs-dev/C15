#pragma once

#include "RPCActionManager.h"

namespace UNDO
{
  class Scope;
}

class UndoActions : public RPCActionManager
{
  typedef RPCActionManager super;

 public:
  UndoActions(UNDO::Scope &scope);
  virtual ~UndoActions();
};
