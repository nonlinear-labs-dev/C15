#pragma once

#include "BaseUnitPresetsAndBanksMode.h"

class Application;

class BaseUnitBanksMode : public BaseUnitPresetsAndBanksMode
{
 private:
  typedef BaseUnitPresetsAndBanksMode super;
 public:
  void setup() override;
};
