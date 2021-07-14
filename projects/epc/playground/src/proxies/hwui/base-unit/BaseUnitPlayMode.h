#pragma once

#include "proxies/hwui/UsageMode.h"
#include "NoteShiftStateMachine.h"
#include <proxies/hwui/ShortVsLongPress.h>

class BaseUnitPlayMode : public UsageMode
{
 private:
  typedef UsageMode super;

 public:
  BaseUnitPlayMode();
  void setup() override;

 protected:
  virtual void toggleTouchBehaviour();

 private:
  void setupBaseUnitUIModeButton();
  void setupBaseUnitMinusButton();
  void setupBaseUnitPlusButton();
  void modeButtonShortPress();
  void modeButtonLongPress();

  std::unordered_map<Buttons, bool> m_buttonStates;
  NoteShiftStateMachine m_noteShiftState;
  ShortVsLongPress m_modeButtonHandler;
  bool checkPanicAffenGriff(Buttons b, bool state);
};
