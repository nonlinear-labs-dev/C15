#pragma once

#include "proxies/hwui/controls/DotSlider.h"

class Application;

class SelectedMacroControlsHWSourceSlider : public DotSlider
{
 private:
  typedef DotSlider super;

 public:
  SelectedMacroControlsHWSourceSlider(const Rect& rect);

 protected:
  virtual void onParamValueChanged(const Parameter* param) override;
  virtual void drawSliderBackground(FrameBuffer& fb) override;

 private:
  void onParameterSelected(Parameter* newOne, SignalOrigin signalType);
  void onMCChanged(const Parameter* param);

  sigc::connection m_mcChanged;
  ParameterId m_hwParamID;
};
