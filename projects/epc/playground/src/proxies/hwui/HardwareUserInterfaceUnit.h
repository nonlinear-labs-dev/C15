#pragma once

#include "playground.h"
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/buttons.h>
#include <memory>

class UsageMode;

class HardwareUserInterfaceUnit
{
 private:
 public:
  HardwareUserInterfaceUnit();
  virtual ~HardwareUserInterfaceUnit();

  HardwareUserInterfaceUnit(const HardwareUserInterfaceUnit& other) = delete;
  HardwareUserInterfaceUnit& operator=(const HardwareUserInterfaceUnit&) = delete;

  virtual void setupFocusAndMode(FocusAndMode focusAndMode);
  virtual bool onButtonPressed(Buttons buttonID, ButtonModifiers modifiers, bool state);

  void setUsageMode(UsageMode* mode);
  void restoreUsageMode(std::shared_ptr<UsageMode> mode);
  [[nodiscard]] std::shared_ptr<UsageMode> getUsageMode() const;
  [[nodiscard]] std::shared_ptr<UsageMode> getScreenSaverUsageMode() const;

 private:
  void onScreenSaverStateChanged(bool state);
  bool m_screenSaverActive = false;
  std::shared_ptr<UsageMode> m_usageMode;
};
