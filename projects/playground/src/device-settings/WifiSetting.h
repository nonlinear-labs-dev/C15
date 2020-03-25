#pragma once
#include "BooleanSetting.h"

#include <thread>
#include <condition_variable>

class WifiSetting : public BooleanSetting
{
 public:
  typedef BooleanSetting super;
  explicit WifiSetting(UpdateDocumentContributor& settings);

  bool set(tEnum m) override;
  static bool pollAccessPointRunning();
};
