#pragma once

#include "proxies/hwui/controls/SelectedParameterValue.h"
#include <nltools/threading/Expiration.h>

class SelectedParamValueWithFrame : public SelectedParameterValue
{
 private:
  typedef SelectedParameterValue super;

 public:
  SelectedParamValueWithFrame(const Rect& rect);
  virtual ~SelectedParamValueWithFrame();

  bool redraw(FrameBuffer& fb) override;

 protected:
  virtual void onModifiersChanged(ButtonModifiers mods) override;

 private:
  SelectedParamValueWithFrame(const SelectedParamValueWithFrame& other) = delete;
  SelectedParamValueWithFrame& operator=(const SelectedParamValueWithFrame&) = delete;

  std::shared_ptr<Font> getFont() const override;
  int getFontHeight() const override;

  void setSuffixFontColor(FrameBuffer& fb) const override;
  void onParameterSelected(Parameter* parameter) override;
  void onParamValueChanged(const Parameter* param) override;
  void showName();

  Expiration m_showParameterName;
  bool m_ignoreNextValueChange = true;
  bool m_ignoreNextModifierChange = true;
};
