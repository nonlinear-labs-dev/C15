#pragma once

#include "playground.h"
#include "Ribbon.h"
#include <sigc++/connection.h>

class Application;
class Parameter;
class Setting;

class UpperRibbon : public Ribbon
{
  typedef Ribbon super;

 public:
  UpperRibbon();
  ~UpperRibbon();

 private:
  void onParamSelectionChanged(Parameter* oldOne, Parameter* newOne);
  void onParamValueChanged(const Parameter* param);

  void onSettingChanged(const Setting* setting);
  void reconnect();
  Parameter* getResponsibleParameter();

  int posToLedID(int pos) const;  // 0 -> 32 ... 16 -> 0

  sigc::connection m_paramConnection;

  sigc::connection m_parameterSelectionChangedSignal;
  sigc::connection m_settingChangedSignal;
};
