#include <nltools/logging/Log.h>
#include "NoteShiftStateMachine.h"
#include "Application.h"
#include "device-settings/Settings.h"
#include "device-settings/NoteShift.h"

NoteShiftStateMachine::NoteShiftStateMachine()
    :

    m_plusButtonState(false)
    , m_minusButtonState(false)
{
  m_resetTimeout.setCallback(
      std::bind(&NoteShiftStateMachine::traverse, this, NoteShiftEvents::NOTE_SHIFT_EVENT_RESET_TIMEOUT));

  registerTraversal(NoteShiftStates::NOTE_SHIFT_STATE_INITIAL, NoteShiftEvents::NOTE_SHIFT_EVENT_PLUS_PRESSED,
                    NoteShiftStates::NOTE_SHIFT_STATE_WAIT_FOR_INC_OCTAVE_OR_DEC_SEMITONE);

  registerTraversal(NoteShiftStates::NOTE_SHIFT_STATE_WAIT_FOR_INC_OCTAVE_OR_DEC_SEMITONE,
                    NoteShiftEvents::NOTE_SHIFT_EVENT_PLUS_RELEASED, NoteShiftStates::NOTE_SHIFT_STATE_INC_OCTAVE);

  registerTraversal(NoteShiftStates::NOTE_SHIFT_STATE_WAIT_FOR_INC_OCTAVE_OR_DEC_SEMITONE,
                    NoteShiftEvents::NOTE_SHIFT_EVENT_MINUS_PRESSED, NoteShiftStates::NOTE_SHIFT_STATE_DEC_SEMITONE);

  registerTraversal(NoteShiftStates::NOTE_SHIFT_STATE_DEC_SEMITONE, NoteShiftEvents::NOTE_SHIFT_EVENT_MINUS_RELEASED,
                    NoteShiftStates::NOTE_SHIFT_STATE_WAIT_FOR_DEC_SEMITONE);

  registerTraversal(NoteShiftStates::NOTE_SHIFT_STATE_DEC_SEMITONE, NoteShiftEvents::NOTE_SHIFT_EVENT_RESET_TIMEOUT,
                    NoteShiftStates::NOTE_SHIFT_STATE_RESET);

  registerTraversal(NoteShiftStates::NOTE_SHIFT_STATE_DEC_SEMITONE, NoteShiftEvents::NOTE_SHIFT_EVENT_PLUS_RELEASED,
                    NoteShiftStates::NOTE_SHIFT_STATE_WAIT_FOR_DEC_OCTAVE_OR_INC_SEMITONE);

  registerTraversal(NoteShiftStates::NOTE_SHIFT_STATE_WAIT_FOR_DEC_SEMITONE,
                    NoteShiftEvents::NOTE_SHIFT_EVENT_MINUS_PRESSED, NoteShiftStates::NOTE_SHIFT_STATE_DEC_SEMITONE);

  registerTraversal(NoteShiftStates::NOTE_SHIFT_STATE_WAIT_FOR_DEC_SEMITONE,
                    NoteShiftEvents::NOTE_SHIFT_EVENT_PLUS_RELEASED, NoteShiftStates::NOTE_SHIFT_STATE_INITIAL);

  registerTraversal(NoteShiftStates::NOTE_SHIFT_STATE_INITIAL, NoteShiftEvents::NOTE_SHIFT_EVENT_MINUS_PRESSED,
                    NoteShiftStates::NOTE_SHIFT_STATE_WAIT_FOR_DEC_OCTAVE_OR_INC_SEMITONE);

  registerTraversal(NoteShiftStates::NOTE_SHIFT_STATE_WAIT_FOR_DEC_OCTAVE_OR_INC_SEMITONE,
                    NoteShiftEvents::NOTE_SHIFT_EVENT_MINUS_RELEASED, NoteShiftStates::NOTE_SHIFT_STATE_DEC_OCTAVE);

  registerTraversal(NoteShiftStates::NOTE_SHIFT_STATE_WAIT_FOR_DEC_OCTAVE_OR_INC_SEMITONE,
                    NoteShiftEvents::NOTE_SHIFT_EVENT_PLUS_PRESSED, NoteShiftStates::NOTE_SHIFT_STATE_INC_SEMITONE);

  registerTraversal(NoteShiftStates::NOTE_SHIFT_STATE_INC_SEMITONE, NoteShiftEvents::NOTE_SHIFT_EVENT_PLUS_RELEASED,
                    NoteShiftStates::NOTE_SHIFT_STATE_WAIT_FOR_INC_SEMITONE);

  registerTraversal(NoteShiftStates::NOTE_SHIFT_STATE_INC_SEMITONE, NoteShiftEvents::NOTE_SHIFT_EVENT_RESET_TIMEOUT,
                    NoteShiftStates::NOTE_SHIFT_STATE_RESET);

  registerTraversal(NoteShiftStates::NOTE_SHIFT_STATE_INC_SEMITONE, NoteShiftEvents::NOTE_SHIFT_EVENT_MINUS_RELEASED,
                    NoteShiftStates::NOTE_SHIFT_STATE_WAIT_FOR_INC_OCTAVE_OR_DEC_SEMITONE);

  registerTraversal(NoteShiftStates::NOTE_SHIFT_STATE_WAIT_FOR_INC_SEMITONE,
                    NoteShiftEvents::NOTE_SHIFT_EVENT_PLUS_PRESSED, NoteShiftStates::NOTE_SHIFT_STATE_INC_SEMITONE);

  registerTraversal(NoteShiftStates::NOTE_SHIFT_STATE_WAIT_FOR_INC_SEMITONE,
                    NoteShiftEvents::NOTE_SHIFT_EVENT_PLUS_RELEASED, NoteShiftStates::NOTE_SHIFT_STATE_INITIAL);

  registerTraversal(NoteShiftStates::NOTE_SHIFT_STATE_RESET, NoteShiftEvents::NOTE_SHIFT_EVENT_PLUS_RELEASED,
                    NoteShiftStates::NOTE_SHIFT_STATE_INITIAL);

  registerTraversal(NoteShiftStates::NOTE_SHIFT_STATE_RESET, NoteShiftEvents::NOTE_SHIFT_EVENT_MINUS_RELEASED,
                    NoteShiftStates::NOTE_SHIFT_STATE_INITIAL);

  registerTraversal(NoteShiftStates::NOTE_SHIFT_STATE_RESET_DONE, NoteShiftEvents::NOTE_SHIFT_EVENT_MINUS_RELEASED,
                    NoteShiftStates::NOTE_SHIFT_STATE_WAIT_FOR_INC_OCTAVE_OR_DEC_SEMITONE);

  registerTraversal(NoteShiftStates::NOTE_SHIFT_STATE_RESET_DONE, NoteShiftEvents::NOTE_SHIFT_EVENT_PLUS_RELEASED,
                    NoteShiftStates::NOTE_SHIFT_STATE_WAIT_FOR_DEC_OCTAVE_OR_INC_SEMITONE);

  registerTraversal(NoteShiftStates::NOTE_SHIFT_STATE_WAIT_FOR_INC_SEMITONE,
                    NoteShiftEvents::NOTE_SHIFT_EVENT_MINUS_RELEASED, NoteShiftStates::NOTE_SHIFT_STATE_INITIAL);

  registerTraversal(NoteShiftStates::NOTE_SHIFT_STATE_WAIT_FOR_INC_SEMITONE,
                    NoteShiftEvents::NOTE_SHIFT_EVENT_PLUS_RELEASED, NoteShiftStates::NOTE_SHIFT_STATE_INITIAL);

  registerTraversal(NoteShiftStates::NOTE_SHIFT_STATE_WAIT_FOR_DEC_SEMITONE,
                    NoteShiftEvents::NOTE_SHIFT_EVENT_MINUS_RELEASED, NoteShiftStates::NOTE_SHIFT_STATE_INITIAL);

  registerTraversal(NoteShiftStates::NOTE_SHIFT_STATE_WAIT_FOR_DEC_SEMITONE,
                    NoteShiftEvents::NOTE_SHIFT_EVENT_PLUS_RELEASED, NoteShiftStates::NOTE_SHIFT_STATE_INITIAL);
}

bool NoteShiftStateMachine::traverse(NoteShiftEvents event)
{
  switch(event)
  {
    case NoteShiftEvents::NOTE_SHIFT_EVENT_PLUS_PRESSED:
      m_plusButtonState = true;
      break;

    case NoteShiftEvents::NOTE_SHIFT_EVENT_PLUS_RELEASED:
      m_plusButtonState = false;
      break;

    case NoteShiftEvents::NOTE_SHIFT_EVENT_MINUS_PRESSED:
      m_minusButtonState = true;
      break;

    case NoteShiftEvents::NOTE_SHIFT_EVENT_MINUS_RELEASED:
      m_minusButtonState = false;
      break;
  }

  bool ret = super::traverse(event);

  if(!m_minusButtonState && !m_plusButtonState)
  {
    ret = setState(NoteShiftStates::NOTE_SHIFT_STATE_INITIAL);
  }
  return ret;
}

bool NoteShiftStateMachine::setState(NoteShiftStates s)
{
  m_resetTimeout.cancel();

  switch(s)
  {
    case NoteShiftStates::NOTE_SHIFT_STATE_INITIAL:
      m_preventOctaveChange = false;
      break;

    case NoteShiftStates::NOTE_SHIFT_STATE_WAIT_FOR_INC_OCTAVE_OR_DEC_SEMITONE:
      break;

    case NoteShiftStates::NOTE_SHIFT_STATE_WAIT_FOR_INC_SEMITONE:
      break;

    case NoteShiftStates::NOTE_SHIFT_STATE_INC_SEMITONE:
      Application::get().getSettings()->getSetting<NoteShift>()->incSemiTone();
      m_preventOctaveChange = true;
      m_resetTimeout.refresh(std::chrono::seconds(1));
      break;

    case NoteShiftStates::NOTE_SHIFT_STATE_INC_OCTAVE:
      if(!m_preventOctaveChange)
      {
        Application::get().getSettings()->getSetting<NoteShift>()->incOctave();
        s = NoteShiftStates::NOTE_SHIFT_STATE_INITIAL;
      }
      break;

    case NoteShiftStates::NOTE_SHIFT_STATE_WAIT_FOR_DEC_OCTAVE_OR_INC_SEMITONE:
      break;

    case NoteShiftStates::NOTE_SHIFT_STATE_DEC_SEMITONE:
      Application::get().getSettings()->getSetting<NoteShift>()->decSemiTone();
      m_preventOctaveChange = true;
      m_resetTimeout.refresh(std::chrono::seconds(1));
      break;

    case NoteShiftStates::NOTE_SHIFT_STATE_WAIT_FOR_DEC_SEMITONE:
      break;

    case NoteShiftStates::NOTE_SHIFT_STATE_DEC_OCTAVE:
      if(!m_preventOctaveChange)
      {
        Application::get().getSettings()->getSetting<NoteShift>()->decOctave();
        s = NoteShiftStates::NOTE_SHIFT_STATE_INITIAL;
      }
      break;

    case NoteShiftStates::NOTE_SHIFT_STATE_RESET:
      Application::get().getSettings()->getSetting<NoteShift>()->set(0);
      s = NoteShiftStates::NOTE_SHIFT_STATE_RESET_DONE;
      break;

    case NoteShiftStates::NOTE_SHIFT_STATE_INVALID:
      break;

    case NoteShiftStates::NOTE_SHIFT_STATE_RESET_DONE:
      break;
  }

  return super::setState(s);
}
