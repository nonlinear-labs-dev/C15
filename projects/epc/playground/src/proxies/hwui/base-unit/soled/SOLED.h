#pragma once

#include "proxies/hwui/OLEDProxy.h"

class Application;
class Setting;

class SOLED : public OLEDProxy
{
  typedef OLEDProxy super;

 public:
  SOLED();
  ~SOLED() override;
  void init();
  void resetSplash();

 private:
  void onBaseUnitUIModeChange(const Setting *s);
  void toggleScreenSaver(bool enable);
};
