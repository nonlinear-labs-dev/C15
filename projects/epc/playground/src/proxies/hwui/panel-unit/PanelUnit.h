#pragma once

#include "EditPanel.h"
#include "proxies/hwui/HardwareUserInterfaceUnit.h"
#include "MacroControlAssignmentStateMachine.h"

class Application;
class Parameter;
class TwoStateLED;

class PanelUnit : public HardwareUserInterfaceUnit, public sigc::trackable
{
 private:
  typedef HardwareUserInterfaceUnit super;

 public:
  typedef std::shared_ptr<TwoStateLED> tLed;

  PanelUnit();
  ~PanelUnit() override;

  void setupFocusAndMode(FocusAndMode focusAndMode) override;
  bool onButtonPressed(Buttons buttonID, ButtonModifiers modifiers, bool state) override;

  void setupFocusAndMode();
  void onTimeout();
  tLed getLED(Buttons id);

  const std::vector<tLed> &getLeds();
  void init();
  void turnLedsOff();

  const EditPanel &getEditPanel() const;
  EditPanel &getEditPanel();
  MacroControlAssignmentStateMachine &getMacroControlAssignmentStateMachine();

 private:
  void installUsageMode(FocusAndMode focusAndMode);
  void initModulateableParameterLayout(Layout *l);
  void onBBBBConnected();
  ParameterId choseHWBestSourceForMC(const ParameterId &mcParamId) const;

  EditPanel m_editPanel;
  std::vector<tLed> m_leds;
  MacroControlAssignmentStateMachine m_macroControlAssignmentStateMachine;

  std::shared_ptr<UsageMode> m_overlayUsageMode;

  sigc::connection m_signalInitializeInstalledLayoutOnce;
};
