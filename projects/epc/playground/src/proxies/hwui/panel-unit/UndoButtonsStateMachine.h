#pragma once

#include "tools/StateMachine.h"
#include <nltools/Uncopyable.h>

enum UndoButtonEvents
{
  UNDO_PRESSED,
  UNDO_RELEASED,
  REDO_PRESSED,
  REDO_RELEASED,
};

enum UndoStates
{
  UNDO_STATE_INITIAL,
  UNDO_STATE_WAIT_FOR_UNDO_OR_SCREEN,
  UNDO_STATE_WAIT_FOR_REDO_OR_SCREEN,
  UNDO_STATE_UNDO,
  UNDO_STATE_REDO,
  UNDO_STATE_SCREEN,
  UNDO_STATE_INVALID
};

class Application;

class UndoButtonStateMachine
    : public StateMachine<UndoStates, UndoButtonEvents, UNDO_STATE_INITIAL, UNDO_STATE_INVALID>,
      public Uncopyable
{
 private:
  typedef StateMachine<UndoStates, UndoButtonEvents, UNDO_STATE_INITIAL, UNDO_STATE_INVALID> super;

 public:
  UndoButtonStateMachine();
  ~UndoButtonStateMachine() override;

 private:
  bool setState(UndoStates s) override;
};
