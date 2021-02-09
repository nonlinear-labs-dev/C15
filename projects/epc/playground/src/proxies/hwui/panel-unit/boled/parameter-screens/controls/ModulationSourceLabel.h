#pragma once

#include "proxies/hwui/controls/LabelRegular8.h"
#include "parameters/Parameter.h"

class Application;
class Parameter;

class ModulationSourceLabel : public Label
{
 private:
  typedef Label super;

 public:
  ModulationSourceLabel(const Rect &r, Font::Justification justification = Font::Justification::Center);
  virtual ~ModulationSourceLabel();

  bool redraw(FrameBuffer &fb) override;
  void setDimmed();

  Font::Justification getJustification() const override;

 private:
  void onParameterSelected(Parameter *parameter, SignalOrigin signalType);
  void onParamValueChanged(const Parameter *param);

  sigc::connection m_paramValueConnection;
  Font::Justification m_justification;
};
