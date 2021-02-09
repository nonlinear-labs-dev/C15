#pragma once

#include "proxies/hwui/controls/DotSlider.h"

class Application;

class SelectedParamsMacroControlSlider : public DotSlider
{
 private:
  typedef DotSlider super;

 public:
  // public methods
  SelectedParamsMacroControlSlider(const Rect& rect);
  virtual ~SelectedParamsMacroControlSlider();

  bool isVisible() const override;
  void setParameter(Parameter* param, SignalOrigin signalType) override;

 private:
  // private methods
  SelectedParamsMacroControlSlider(const SelectedParamsMacroControlSlider& other);
  SelectedParamsMacroControlSlider& operator=(const SelectedParamsMacroControlSlider&);

  void onTargetParamValueChanged(const Parameter* param);
  void setTargetParameter(Parameter* param, SignalOrigin signalType);

  Parameter* m_targetParameter = nullptr;
  sigc::connection m_targetParamValueConnection;
};
