#pragma once

#include "proxies/hwui/controls/ControlWithChildren.h"
#include <sigc++/connection.h>

class Application;
class Parameter;
class Label;
class Slider;
class MacroControlParameter;

class MacroControl : public ControlWithChildren
{
 private:
  typedef ControlWithChildren super;

 public:
  // public methods
  MacroControl(const Rect &pos);
  virtual ~MacroControl();

 private:
  // private methods
  MacroControl(const MacroControl &other);
  MacroControl &operator=(const MacroControl &);

  void onSelectionChanged(Parameter *parameter, SignalOrigin signalType);
  void onTargetChanged(const Parameter *modulatingParameter);

  Label *m_name = nullptr;
  Slider *m_slider = nullptr;
  Label *m_value = nullptr;

  sigc::connection m_connectionTarget;
  MacroControlParameter *m_srcParam = nullptr;
};
