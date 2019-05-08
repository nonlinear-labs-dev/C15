#pragma once

#include "proxies/hwui/controls/Label.h"

class Application;
class Parameter;

class SelectedMacroControlsHWSourceName : public Label
{
 private:
  typedef Label super;

 public:
  // public methods
  SelectedMacroControlsHWSourceName(const Rect& rect);
  virtual ~SelectedMacroControlsHWSourceName();

 private:
  // private methods
  SelectedMacroControlsHWSourceName(const SelectedMacroControlsHWSourceName& other);
  SelectedMacroControlsHWSourceName& operator=(const SelectedMacroControlsHWSourceName&);

  void onParameterSelected(Parameter* newOne);
  void onMCChanged(const Parameter* param);

  int m_hwParamID = 0;
  sigc::connection m_mcChanged;
};
