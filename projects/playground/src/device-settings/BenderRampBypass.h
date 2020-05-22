#pragma once

#include <device-settings/BooleanSetting.h>

class BenderRampBypass : public BooleanSetting
{
 public:
  BenderRampBypass(Settings& settings);
  virtual ~BenderRampBypass();

 private:
  void sendToLPC(SendReason reason) const override;
  bool persistent() const override;
};
