#pragma once

#include <tools/StateMachine.h>
#include <tools/Expiration.h>

enum NoteShiftEvents
{
  NOTE_SHIFT_EVENT_PLUS_PRESSED,
  NOTE_SHIFT_EVENT_PLUS_RELEASED,
  NOTE_SHIFT_EVENT_MINUS_PRESSED,
  NOTE_SHIFT_EVENT_MINUS_RELEASED,
  NOTE_SHIFT_EVENT_RESET_TIMEOUT
};

enum NoteShiftStates
{
  NOTE_SHIFT_STATE_INITIAL,
  NOTE_SHIFT_STATE_WAIT_FOR_INC_OCTAVE_OR_DEC_SEMITONE,
  NOTE_SHIFT_STATE_WAIT_FOR_INC_SEMITONE,
  NOTE_SHIFT_STATE_INC_SEMITONE,
  NOTE_SHIFT_STATE_INC_OCTAVE,
  NOTE_SHIFT_STATE_WAIT_FOR_DEC_OCTAVE_OR_INC_SEMITONE,
  NOTE_SHIFT_STATE_DEC_SEMITONE,
  NOTE_SHIFT_STATE_WAIT_FOR_DEC_SEMITONE,
  NOTE_SHIFT_STATE_DEC_OCTAVE,
  NOTE_SHIFT_STATE_RESET,
  NOTE_SHIFT_STATE_INVALID
};

class Application;

class NoteShiftStateMachine
    : public StateMachine<NoteShiftStates, NoteShiftEvents, NOTE_SHIFT_STATE_INITIAL, NOTE_SHIFT_STATE_INVALID>
{
 private:
  typedef StateMachine<NoteShiftStates, NoteShiftEvents, NOTE_SHIFT_STATE_INITIAL, NOTE_SHIFT_STATE_INVALID> super;

 public:
  NoteShiftStateMachine();
  virtual ~NoteShiftStateMachine();

  bool traverse(NoteShiftEvents event) override;

 private:
  bool setState(NoteShiftStates s) override;

  NoteShiftStateMachine(const NoteShiftStateMachine& other) = delete;
  NoteShiftStateMachine& operator=(const NoteShiftStateMachine&) = delete;

  bool m_plusButtonState;
  bool m_minusButtonState;
  Expiration m_resetTimeout;
};
