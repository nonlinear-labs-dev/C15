#pragma once

#include "proxies/hwui/controls/DotSlider.h"

class Application;

class SelectedMacroControlsHWSourceSlider : public DotSlider
{
 private:
  typedef DotSlider super;

 public:
  explicit SelectedMacroControlsHWSourceSlider(const Rect& rect);

 protected:
  void onParamValueChanged(const Parameter* param) override;
  void drawSliderBackground(FrameBuffer& fb) override;

 private:
  void onParameterSelected(Parameter* newOne);
  void onMCChanged(const Parameter* param);

  sigc::connection m_mcChanged;
  ParameterId m_hwParamID;
};
