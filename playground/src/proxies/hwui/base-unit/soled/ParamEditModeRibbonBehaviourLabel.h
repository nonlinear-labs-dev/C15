#pragma once

#include "proxies/hwui/controls/Label.h"

class Application;
class Setting;
class Settings;
class ParameterEditModeRibbonBehaviour;

class ParamEditModeRibbonBehaviourLabel : public Label
{
 private:
  typedef Label super;

 public:
  ParamEditModeRibbonBehaviourLabel(const Rect& rect);
  virtual ~ParamEditModeRibbonBehaviourLabel();

 private:
  ParamEditModeRibbonBehaviourLabel(const ParamEditModeRibbonBehaviourLabel& other) = delete;
  ParamEditModeRibbonBehaviourLabel& operator=(const ParamEditModeRibbonBehaviourLabel&) = delete;

  void updateText(const Setting* s);
  std::shared_ptr<ParameterEditModeRibbonBehaviour> getSetting() const;
  Settings* getSettings() const;
};
