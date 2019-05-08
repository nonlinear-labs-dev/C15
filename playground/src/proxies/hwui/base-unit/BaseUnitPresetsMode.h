#pragma once

#include "BaseUnitPresetsAndBanksMode.h"

class Application;

class BaseUnitPresetsMode : public BaseUnitPresetsAndBanksMode
{
 private:
  typedef BaseUnitPresetsAndBanksMode super;

 public:
  BaseUnitPresetsMode();
  virtual ~BaseUnitPresetsMode();

  void setup() override;

 private:
  void onFuncButtonDown() override;
};
