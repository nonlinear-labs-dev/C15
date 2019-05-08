#pragma once

#include "BaseUnitPresetsAndBanksMode.h"

class Application;

class BaseUnitBanksMode : public BaseUnitPresetsAndBanksMode
{
 private:
  typedef BaseUnitPresetsAndBanksMode super;

 public:
  BaseUnitBanksMode();
  virtual ~BaseUnitBanksMode();

  void setup() override;
};
