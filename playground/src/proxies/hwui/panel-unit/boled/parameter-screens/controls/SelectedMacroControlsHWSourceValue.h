#pragma once

#include "proxies/hwui/controls/Label.h"

class Application;
class Parameter;

class SelectedMacroControlsHWSourceValue : public Label
{
 private:
  typedef Label super;

 public:
  SelectedMacroControlsHWSourceValue(const Rect &rect);
  virtual ~SelectedMacroControlsHWSourceValue();

 private:
  SelectedMacroControlsHWSourceValue(const SelectedMacroControlsHWSourceValue &other);
  SelectedMacroControlsHWSourceValue &operator=(const SelectedMacroControlsHWSourceValue &);

  void onModifiersChanged();
  void onParameterSelected(Parameter *newOne);
  void onMCChanged(const Parameter *param);
  void updateText(const Parameter *param);
  virtual void setSuffixFontColor(FrameBuffer &fb) const override;
  int getHWSourceID(const Parameter *param) const;

  int m_hwParamID = 0;
  sigc::connection m_mcChanged;
  sigc::connection m_hwChanged;
};
