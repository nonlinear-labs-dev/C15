#pragma once

#include "proxies/hwui/controls/Label.h"
#include "parameters/Parameter.h"

class Application;
class Parameter;

class SelectedParameterValue : public Label
{
 private:
  typedef Label super;

 public:
  explicit SelectedParameterValue(const Rect &rect);
  ~SelectedParameterValue() override;

 protected:
  virtual void onParameterSelected(Parameter *parameter);
  virtual void onParamValueChanged(const Parameter *param);

  void setSuffixFontColor(FrameBuffer &fb) const override;
  virtual void onModifiersChanged(ButtonModifiers mods);

  void onVoiceGroupSelectionChanged(VoiceGroup v);
  void onSoundTypeChanged();

  bool redraw(FrameBuffer &fb) override;

 private:
  sigc::connection m_paramValueConnection;
};
