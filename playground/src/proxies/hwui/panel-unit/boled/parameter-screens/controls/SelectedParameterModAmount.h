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
  // public methods
  SelectedParameterModAmount(const Rect& rect);
  virtual ~SelectedParameterModAmount();

  virtual bool redraw(FrameBuffer& fb) override;
  void setRange(float from, float to);

 protected:
  virtual void setSliderColor(FrameBuffer& fb);

 private:
  // private methods
  SelectedParameterModAmount(const SelectedParameterModAmount& other);
  SelectedParameterModAmount& operator=(const SelectedParameterModAmount&);

  void setRangeOrdered(float from, float to);
  void setParameter(Parameter* param);
  void onParamValueChanged(const Parameter* param);

  sigc::connection m_paramValueConnection;

  float m_from;
  float m_to;
};
