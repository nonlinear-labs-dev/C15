#pragma once

#include "proxies/hwui/controls/DotSlider.h"

class Application;

class SelectedParamsMacroControlSlider : public DotSlider
{
 private:
  typedef DotSlider super;

 public:
  explicit SelectedParamsMacroControlSlider(const Rect& rect);
  ~SelectedParamsMacroControlSlider() override;

  bool isVisible() const override;
  void setParameter(Parameter* param) override;

  SelectedParamsMacroControlSlider(const SelectedParamsMacroControlSlider& other) = delete;
  SelectedParamsMacroControlSlider& operator=(const SelectedParamsMacroControlSlider&) = delete;

 private:
  void onTargetParamValueChanged(const Parameter* param);
  void setTargetParameter(Parameter* param);

  Parameter* m_targetParameter = nullptr;
  sigc::connection m_targetParamValueConnection;
};
