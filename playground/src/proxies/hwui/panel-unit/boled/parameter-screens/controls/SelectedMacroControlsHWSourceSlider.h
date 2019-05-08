#pragma once

#include "proxies/hwui/controls/DotSlider.h"

class Application;

class SelectedMacroControlsHWSourceSlider : public DotSlider
{
 private:
  typedef DotSlider super;

 public:
  SelectedMacroControlsHWSourceSlider(const Rect& rect);
  virtual ~SelectedMacroControlsHWSourceSlider();

 protected:
  virtual void onParamValueChanged(const Parameter* param) override;
  virtual void drawSliderBackground(FrameBuffer& fb) override;

 private:
  SelectedMacroControlsHWSourceSlider(const SelectedMacroControlsHWSourceSlider& other);
  SelectedMacroControlsHWSourceSlider& operator=(const SelectedMacroControlsHWSourceSlider&);

  void onParameterSelected(Parameter* newOne);
  void onMCChanged(const Parameter* param);

  sigc::connection m_mcChanged;
  int m_hwParamID;
};
