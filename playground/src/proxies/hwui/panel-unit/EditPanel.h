#pragma once

#include "RotaryEncoder.h"
#include "boled/BOLED.h"
#include "UndoButtonsStateMachine.h"

class Application;
class BOLED;

class EditPanel
{
 public:
  EditPanel();
  virtual ~EditPanel();

  void init();

  void onTimeout();
  RotaryEncoder &getKnob();
  BOLED &getBoled();
  const BOLED &getBoled() const;

  bool onButtonPressed(Buttons buttonID, ButtonModifiers modifiers, bool state);
  void setupFocusAndMode(FocusAndMode focusAndMode);
  UndoButtonStateMachine &getUndoStateMachine();

 private:
  BOLED m_oled;
  RotaryEncoder m_knob;
  UndoButtonStateMachine m_statemachine;
};
