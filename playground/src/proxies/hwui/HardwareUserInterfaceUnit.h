#pragma once

#include "playground.h"
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/buttons.h>

class UsageMode;

class HardwareUserInterfaceUnit
{
  private:

  public:
    HardwareUserInterfaceUnit ();
    virtual ~HardwareUserInterfaceUnit ();

    virtual void setupFocusAndMode (FocusAndMode focusAndMode);

    void setUsageMode (UsageMode *mode);
    void restoreUsageMode (shared_ptr<UsageMode> mode);
    shared_ptr<UsageMode> getUsageMode ();
    shared_ptr<const UsageMode> getUsageMode () const;

    virtual bool onButtonPressed(Buttons buttonID, ButtonModifiers modifiers, bool state);

  private:
    HardwareUserInterfaceUnit (const HardwareUserInterfaceUnit& other) = delete;
    HardwareUserInterfaceUnit& operator= (const HardwareUserInterfaceUnit&) = delete;

    shared_ptr<UsageMode> m_usageMode;
};

