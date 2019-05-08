#pragma once

#include "proxies/hwui/UsageMode.h"
#include "NoteShiftStateMachine.h"

class Application;

class BaseUnitPlayMode : public UsageMode
{
 private:
  typedef UsageMode super;

 public:
  BaseUnitPlayMode();
  virtual ~BaseUnitPlayMode();

  void setup() override;

 protected:
  virtual void toggleTouchBehaviour();

 private:
  void setupBaseUnitUIModeButton();
  void setupBaseUnitMinusButton();
  void setupBaseUnitPlusButton();

  NoteShiftStateMachine m_noteShiftState;
};
