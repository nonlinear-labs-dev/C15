#pragma once

#include <proxies/hwui/DFBLayout.h>

class SplashScreenLayout : public DFBLayout
{
 private:
  typedef DFBLayout super;

 public:
  SplashScreenLayout();
  virtual ~SplashScreenLayout();
};
