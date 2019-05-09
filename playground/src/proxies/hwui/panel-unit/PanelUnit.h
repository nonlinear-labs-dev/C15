#pragma once

#include "EditPanel.h"
#include "proxies/hwui/HardwareUserInterfaceUnit.h"
#include <proxies/hwui/TwoStateLED.h>
#include "MacroControlAssignmentStateMachine.h"

class Application;
class Parameter;
class TwoStateLED;

class PanelUnit : public HardwareUserInterfaceUnit, public sigc::trackable
{
 private:
  typedef HardwareUserInterfaceUnit super;

 public:
  PanelUnit();
  virtual ~PanelUnit();

  typedef std::shared_ptr<TwoStateLED> tLed;

  void setupFocusAndMode();
  virtual void setupFocusAndMode(FocusAndMode focusAndMode) override;
  void onTimeout();

  tLed getLED(Buttons id);
  EditPanel &getEditPanel();
  const EditPanel &getEditPanel() const;

  virtual bool onButtonPressed(Buttons buttonID, ButtonModifiers modifiers, bool state) override;
  void init();
  void turnLedsOff();

  MacroControlAssignmentStateMachine &getMacroControlAssignmentStateMachine();

 private:
  void installUsageMode(FocusAndMode focusAndMode);
  int choseHWBestSourceForMC(int mcParamId) const;
  void onBBBBConnected();

  EditPanel m_editPanel;
  std::vector<tLed> m_leds;
  MacroControlAssignmentStateMachine m_macroControlAssignmentStateMachine;
};
