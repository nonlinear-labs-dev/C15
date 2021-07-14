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
  explicit UndoActions(UNDO::Scope &scope);
  ~UndoActions() override;
};
