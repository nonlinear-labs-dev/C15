#pragma once

#include "playground.h"
#include "Ribbon.h"

#include <sigc++/trackable.h>

class Application;
class Parameter;
class Setting;

class LowerRibbon : public Ribbon, public sigc::trackable
{
  typedef Ribbon super;

 public:
  LowerRibbon();
  void indicateBlockingMainThread(bool onOff);

 private:
  void onParamValueChanged(const Parameter* param);

  int posToLedID(int pos) const override;

  bool m_indicateBlockingMainThread = false;
};
