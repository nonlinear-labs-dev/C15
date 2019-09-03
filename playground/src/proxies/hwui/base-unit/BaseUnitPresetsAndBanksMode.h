#pragma once

#include <proxies/hwui/UsageMode.h>
#include <proxies/hwui/ShortVsLongPress.h>
#include <proxies/hwui/ButtonRepeat.h>

class Application;

class BaseUnitPresetsAndBanksMode : public UsageMode
{
 private:
  typedef UsageMode super;

 public:
  BaseUnitPresetsAndBanksMode();
  void setup() override;

 protected:
  void installButtonRepeat(const std::function<void()>& cb);
  void removeButtonRepeat();

 private:
  virtual void onFuncButtonDown();
  void modeButtonShortPress();
  void modeButtonLongPress();

  ShortVsLongPress m_modeButtonHandler;
  ShortVsLongPress m_funcButtonHandler;

  std::unique_ptr<ButtonRepeat> m_buttonRepeat;
};
