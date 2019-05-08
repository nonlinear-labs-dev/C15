#include "NoteShiftStateMachine.h"
#include "Application.h"
#include "device-settings/Settings.h"
#include "device-settings/NoteShift.h"

NoteShiftStateMachine::NoteShiftStateMachine()
    :

    m_plusButtonState(false)
    , m_minusButtonState(false)
{
  m_resetTimeout.setCallback(bind(&NoteShiftStateMachine::traverse, this, NOTE_SHIFT_EVENT_RESET_TIMEOUT));

  registerTraversal(NOTE_SHIFT_STATE_INITIAL, NOTE_SHIFT_EVENT_PLUS_PRESSED,
                    NOTE_SHIFT_STATE_WAIT_FOR_INC_OCTAVE_OR_DEC_SEMITONE);
  registerTraversal(NOTE_SHIFT_STATE_WAIT_FOR_INC_OCTAVE_OR_DEC_SEMITONE, NOTE_SHIFT_EVENT_PLUS_RELEASED,
                    NOTE_SHIFT_STATE_INC_OCTAVE);
  registerTraversal(NOTE_SHIFT_STATE_WAIT_FOR_INC_OCTAVE_OR_DEC_SEMITONE, NOTE_SHIFT_EVENT_MINUS_PRESSED,
                    NOTE_SHIFT_STATE_DEC_SEMITONE);
  registerTraversal(NOTE_SHIFT_STATE_DEC_SEMITONE, NOTE_SHIFT_EVENT_MINUS_RELEASED,
                    NOTE_SHIFT_STATE_WAIT_FOR_DEC_SEMITONE);
  registerTraversal(NOTE_SHIFT_STATE_DEC_SEMITONE, NOTE_SHIFT_EVENT_RESET_TIMEOUT, NOTE_SHIFT_STATE_RESET);
  registerTraversal(NOTE_SHIFT_STATE_WAIT_FOR_DEC_SEMITONE, NOTE_SHIFT_EVENT_MINUS_PRESSED,
                    NOTE_SHIFT_STATE_DEC_SEMITONE);
  registerTraversal(NOTE_SHIFT_STATE_WAIT_FOR_DEC_SEMITONE, NOTE_SHIFT_EVENT_PLUS_RELEASED, NOTE_SHIFT_STATE_INITIAL);

  registerTraversal(NOTE_SHIFT_STATE_INITIAL, NOTE_SHIFT_EVENT_MINUS_PRESSED,
                    NOTE_SHIFT_STATE_WAIT_FOR_DEC_OCTAVE_OR_INC_SEMITONE);
  registerTraversal(NOTE_SHIFT_STATE_WAIT_FOR_DEC_OCTAVE_OR_INC_SEMITONE, NOTE_SHIFT_EVENT_MINUS_RELEASED,
                    NOTE_SHIFT_STATE_DEC_OCTAVE);
  registerTraversal(NOTE_SHIFT_STATE_WAIT_FOR_DEC_OCTAVE_OR_INC_SEMITONE, NOTE_SHIFT_EVENT_PLUS_PRESSED,
                    NOTE_SHIFT_STATE_INC_SEMITONE);
  registerTraversal(NOTE_SHIFT_STATE_INC_SEMITONE, NOTE_SHIFT_EVENT_PLUS_RELEASED,
                    NOTE_SHIFT_STATE_WAIT_FOR_INC_SEMITONE);
  registerTraversal(NOTE_SHIFT_STATE_INC_SEMITONE, NOTE_SHIFT_EVENT_RESET_TIMEOUT, NOTE_SHIFT_STATE_RESET);
  registerTraversal(NOTE_SHIFT_STATE_WAIT_FOR_INC_SEMITONE, NOTE_SHIFT_EVENT_PLUS_PRESSED,
                    NOTE_SHIFT_STATE_INC_SEMITONE);
  registerTraversal(NOTE_SHIFT_STATE_WAIT_FOR_INC_SEMITONE, NOTE_SHIFT_EVENT_PLUS_RELEASED, NOTE_SHIFT_STATE_INITIAL);
}

NoteShiftStateMachine::~NoteShiftStateMachine()
{
}

bool NoteShiftStateMachine::traverse(NoteShiftEvents event)
{
  switch(event)
  {
    case NOTE_SHIFT_EVENT_PLUS_PRESSED:
      m_plusButtonState = true;
      break;

    case NOTE_SHIFT_EVENT_PLUS_RELEASED:
      m_plusButtonState = false;
      break;

    case NOTE_SHIFT_EVENT_MINUS_PRESSED:
      m_minusButtonState = true;
      break;

    case NOTE_SHIFT_EVENT_MINUS_RELEASED:
      m_minusButtonState = false;
      break;
  }

  bool ret = super::traverse(event);

  if(!m_minusButtonState && !m_plusButtonState)
  {
    ret = setState(NOTE_SHIFT_STATE_INITIAL);
  }
  return ret;
}

bool NoteShiftStateMachine::setState(NoteShiftStates s)
{
  m_resetTimeout.cancel();

  switch(s)
  {
    case NOTE_SHIFT_STATE_INITIAL:
      break;

    case NOTE_SHIFT_STATE_WAIT_FOR_INC_OCTAVE_OR_DEC_SEMITONE:
      break;

    case NOTE_SHIFT_STATE_WAIT_FOR_INC_SEMITONE:
      break;

    case NOTE_SHIFT_STATE_INC_SEMITONE:
      Application::get().getSettings()->getSetting<NoteShift>()->incSemiTone();
      m_resetTimeout.refresh(std::chrono::seconds(1));
      break;

    case NOTE_SHIFT_STATE_INC_OCTAVE:
      Application::get().getSettings()->getSetting<NoteShift>()->incOctave();
      s = NOTE_SHIFT_STATE_INITIAL;
      break;

    case NOTE_SHIFT_STATE_WAIT_FOR_DEC_OCTAVE_OR_INC_SEMITONE:
      break;

    case NOTE_SHIFT_STATE_DEC_SEMITONE:
      Application::get().getSettings()->getSetting<NoteShift>()->decSemiTone();
      m_resetTimeout.refresh(std::chrono::seconds(1));
      break;

    case NOTE_SHIFT_STATE_WAIT_FOR_DEC_SEMITONE:
      break;

    case NOTE_SHIFT_STATE_DEC_OCTAVE:
      Application::get().getSettings()->getSetting<NoteShift>()->decOctave();
      s = NOTE_SHIFT_STATE_INITIAL;
      break;

    case NOTE_SHIFT_STATE_RESET:
      Application::get().getSettings()->getSetting<NoteShift>()->set(0);
      s = NOTE_SHIFT_STATE_INITIAL;
      break;

    case NOTE_SHIFT_STATE_INVALID:
      break;
  }

  return super::setState(s);
}
