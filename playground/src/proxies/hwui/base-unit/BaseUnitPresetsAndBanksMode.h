#pragma once

#include <proxies/hwui/UsageMode.h>
#include <tools/Expiration.h>

class Application;

class BaseUnitPresetsAndBanksMode : public UsageMode
{
 private:
  typedef UsageMode super;

 public:
  BaseUnitPresetsAndBanksMode();
  virtual ~BaseUnitPresetsAndBanksMode();

  void setup() override;

 private:
  virtual void onFuncButtonDown();
  void onLongPress();

  Expiration m_fireLongPress;
};
