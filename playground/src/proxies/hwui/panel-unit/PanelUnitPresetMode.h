#pragma once

#include "PanelUnitParameterEditMode.h"
#include "tools/Throttler.h"
#include <array>
#include <list>
#include <parameters/names/ParameterDB.h>


class PanelUnitPresetMode: public PanelUnitParameterEditMode
{
    typedef PanelUnitParameterEditMode super;

  public:
    PanelUnitPresetMode();
    virtual ~PanelUnitPresetMode();
    void bruteForceUpdateLeds() override;

  private:
    Throttler m_bruteForceLedThrottler;

    static constexpr auto numLeds = 96;
    static constexpr auto invalidSignalFlowIndicator = ParameterDB::getInvalidSignalPathIndication();

    void setStateForButton(Buttons buttonId, const list<int> parameters, array<TwoStateLED::LedState, numLeds>& states);
    void applyStateToLeds(array<TwoStateLED::LedState, numLeds>& states);
};
