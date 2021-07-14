#pragma once

#include "tools/StateMachine.h"
#include <sigc++/connection.h>

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
  ~FineButton() override;

  FineButton(const FineButton& other) = delete;
  FineButton& operator=(const FineButton&) = delete;

  void setButtonState(bool pressed);
  [[nodiscard]] bool getModifierState() const;
  bool setState(FineButtonStates s) override;
  bool onTimeout();
  bool onlyTemporary();
  void setShiftedWhilePressDown(bool down);

 private:
  bool m_buttonState;
  bool m_modifierState;
  bool m_shifted;
  sigc::connection m_timeout;
};
