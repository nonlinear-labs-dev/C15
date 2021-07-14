#pragma once

#include "proxies/hwui/controls/Label.h"
#include <ParameterId.h>
#include <sigc++/connection.h>

class Application;
class Parameter;

class SelectedMacroControlsHWSourceName : public Label
{
 private:
  typedef Label super;

 public:
  // public methods
  explicit SelectedMacroControlsHWSourceName(const Rect& rect);
  ~SelectedMacroControlsHWSourceName() override;

 private:
  // private methods
  SelectedMacroControlsHWSourceName(const SelectedMacroControlsHWSourceName& other);
  SelectedMacroControlsHWSourceName& operator=(const SelectedMacroControlsHWSourceName&);

  void onParameterSelected(Parameter* newOne);
  void onMCChanged(const Parameter* param);

  ParameterId m_hwParamID;
  sigc::connection m_mcChanged;
};
