#pragma once

#include "PanelUnitParameterEditMode.h"
#include <nltools/threading/Throttler.h>
#include <array>
#include <list>
#include <parameters/names/ParameterDB.h>
#include <proxies/hwui/TwoStateLED.h>

class PanelUnitPresetMode : public PanelUnitParameterEditMode
{
  typedef PanelUnitParameterEditMode super;

 public:
  PanelUnitPresetMode();
  ~PanelUnitPresetMode() override;
  void bruteForceUpdateLeds() override;

 private:
  Throttler m_bruteForceLedThrottler;

  static constexpr auto numLeds = 96;
  static constexpr auto invalidSignalFlowIndicator = ParameterDB::getInvalidSignalPathIndication();

  void setStateForButton(Buttons buttonId, const std::list<int>& parameters,
                         std::array<TwoStateLED::LedState, numLeds>& states);
  void letChangedButtonsBlink(Buttons buttonId, const std::list<int>& parameters,
                              std::array<TwoStateLED::LedState, numLeds>& states);
  void applyStateToLeds(std::array<TwoStateLED::LedState, numLeds>& states);
  std::pair<bool, bool> trySpecialCaseParameter(const Parameter* selParam) const;
};

class PanelUnitSoundMode : public PanelUnitPresetMode
{
 public:
  PanelUnitSoundMode();

  void setup() override;
};
