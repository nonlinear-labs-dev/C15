#pragma once

#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/ButtonParameterMapping.h>
#include <proxies/hwui/panel-unit/UndoButtonsStateMachine.h>
#include <proxies/hwui/UsageMode.h>
#include <ParameterId.h>
#include <sigc++/connection.h>
#include <array>
#include <list>
#include <vector>

class PresetBank;
class Parameter;
class Layout;
class Layout;
class BOLED;
class MacroControlAssignmentStateMachine;

class PanelUnitParameterEditMode : public UsageMode
{
  typedef UsageMode super;

 public:
  PanelUnitParameterEditMode();
  virtual ~PanelUnitParameterEditMode();

  void setup() override;
  Buttons findButtonForParameter(Parameter *param) const;
  std::list<int> getButtonAssignments(Buttons button) const;
  std::list<int> getButtonAssignments(Buttons button, SoundType type) const;
  virtual void setupFocusAndMode(FocusAndMode focusAndMode) override;

  static const int NUM_LEDS = 96;

  virtual void bruteForceUpdateLeds() override;

 protected:
  ButtonParameterMapping &getMappings();

 private:
  typedef std::array<bool, NUM_LEDS> tLedStates;

  void onParamSelectionChanged(Parameter *oldParam, Parameter *newParam, SignalOrigin signalType);

  bool tryParameterToggleOnMacroControl(std::vector<gint32> ids, Parameter *selParam);

  tAction createParameterSelectAction(std::vector<gint32> toggleAudioIDs);

  bool toggleParameterSelection(std::vector<gint32> ids, bool state);
  bool setParameterSelection(const ParameterId &audioID, bool state);

  bool isShowingParameterScreen() const;
  bool switchToNormalModeInCurrentParameterLayout();

  void collectLedStates(tLedStates &states, ParameterId selectedParameterID);

  void letTargetsBlink(Parameter *selParam);
  void letMacroControlTargetsBlink();
  void letOscAShaperABlink(const std::vector<int> &targets);
  void letOscBShaperBBlink(const std::vector<int> &targets);
  void letOtherTargetsBlink(const std::vector<int> &targets);
  void letReverbBlink(const std::vector<int> &targets);
  bool isSignalFlowingThrough(const Parameter *p) const;
  void setLedStates(const tLedStates &states);

  const BOLED &getBoled() const;
  BOLED &getBoled();
  bool handleMacroControlButton(bool state, int mcParamId);

  MacroControlAssignmentStateMachine &getMacroControlAssignmentStateMachine();

  ButtonParameterMapping m_mappings;

  sigc::connection m_connectionToMacroControl;
};
