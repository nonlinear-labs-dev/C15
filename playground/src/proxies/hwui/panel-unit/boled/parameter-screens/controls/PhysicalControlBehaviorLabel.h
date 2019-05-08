#pragma once

#include <proxies/hwui/controls/Label.h>

class Parameter;
class Application;

class PhysicalControlBehaviorLabel : public Label
{
 private:
  typedef Label super;

 public:
  PhysicalControlBehaviorLabel(const Rect &pos);

 private:
  void setParameter(Parameter *param);
  void onParamValueChanged(const Parameter *param);

  sigc::connection m_paramConnection;
};
