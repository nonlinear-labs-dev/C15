#pragma once

#include "proxies/hwui/controls/Control.h"
#include "parameters/Parameter.h"

class Application;
class Parameter;

class SelectedParameterModAmount : public Control
{
 private:
  typedef Control super;

 public:
  SelectedParameterModAmount(const Rect& rect);

  virtual bool redraw(FrameBuffer& fb) override;
  void setRange(float from, float to);

 protected:
  virtual void setSliderColor(FrameBuffer& fb);

 private:
  void setRangeOrdered(float from, float to);
  void setParameter(Parameter* param, SignalOrigin signalType);
  void onParamValueChanged(const Parameter* param);

  sigc::connection m_paramValueConnection;

  float m_from;
  float m_to;
};
