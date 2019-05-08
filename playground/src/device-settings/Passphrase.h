#pragma once

#include "HostAPDSetting.h"

class Settings;

class Passphrase : public HostAPDSetting
{
 private:
  typedef HostAPDSetting super;

 public:
  Passphrase(Settings &parent);
  virtual ~Passphrase();

  void dice();
};
