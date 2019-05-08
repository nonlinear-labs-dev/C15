#pragma once

#include "PlayPanel.h"
#include "UpperRibbon.h"
#include "LowerRibbon.h"
#include "proxies/hwui/HardwareUserInterfaceUnit.h"

class Application;

class BaseUnit : public HardwareUserInterfaceUnit, public sigc::trackable
{
 private:
  typedef HardwareUserInterfaceUnit super;

 public:
  BaseUnit();
  virtual ~BaseUnit();

  void onTimeout();
  int getLastTouchedRibbon();
  void indicateBlockingMainThread(bool onOff);

  void init();

  PlayPanel &getPlayPanel();

 private:
  void respectUsageMode(const Setting *s);
  void onBBBBConnected();

  PlayPanel m_playPanel;
  UpperRibbon m_upperRibbon;
  LowerRibbon m_lowerRibbon;
};
