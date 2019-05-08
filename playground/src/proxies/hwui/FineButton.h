#pragma once

#include "tools/StateMachine.h"

enum FineButtonEvents
{
  DEPRESSED,
  RELEASED,
  TIME_ELAPSED
};

enum FineButtonStates
{
  TOGGLED_OFF,
  TOGGLED_ON,
  CURRENT_ON,
  TOGGLED_ON_SWITCHING_OFF,
  INVALID
};

class FineButton : public StateMachine<FineButtonStates, FineButtonEvents, TOGGLED_OFF, INVALID>
{
 private:
  typedef StateMachine<FineButtonStates, FineButtonEvents, TOGGLED_OFF, INVALID> super;

 public:
  FineButton();
  virtual ~FineButton();

  void setButtonState(bool pressed);
  bool getModifierState() const;
  bool setState(FineButtonStates s) override;
  bool onTimeout();
  bool onlyTemporary();
  void setShiftedWhilePressDown(bool down);

 private:
  FineButton(const FineButton& other) = delete;
  FineButton& operator=(const FineButton&) = delete;

  bool m_buttonState;
  bool m_modifierState;
  bool m_shifted;
  sigc::connection m_timeout;
};
