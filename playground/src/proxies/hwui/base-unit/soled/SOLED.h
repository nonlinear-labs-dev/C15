#pragma once

#include "proxies/hwui/OLEDProxy.h"

class Application;
class Setting;

class SOLED : public OLEDProxy
{
  typedef OLEDProxy super;

 public:
  SOLED();
  virtual ~SOLED();
  void init();
  void resetSplash();

 private:
  void onBaseUnitUIModeChange(const Setting *s);
};
