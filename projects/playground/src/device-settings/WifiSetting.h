#pragma once

#include "BooleanSetting.h"

class WifiSetting : public BooleanSetting
{
 public:
  typedef BooleanSetting super;
  explicit WifiSetting(UpdateDocumentContributor& settings);

  bool set(tEnum m) override;
  static bool pollAccessPointRunning();
};
