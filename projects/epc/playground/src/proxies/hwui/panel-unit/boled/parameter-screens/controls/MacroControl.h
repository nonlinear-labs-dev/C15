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
  explicit MacroControl(const Rect &pos);
  ~MacroControl() override;
  MacroControl(const MacroControl &other) = delete;
  MacroControl &operator=(const MacroControl &) = delete;

 private:
  void onSelectionChanged(Parameter *parameter);
  void onTargetChanged(const Parameter *modulatingParameter);

  Label *m_name = nullptr;
  Slider *m_slider = nullptr;
  Label *m_value = nullptr;

  sigc::connection m_connectionTarget;
  MacroControlParameter *m_srcParam = nullptr;
};
