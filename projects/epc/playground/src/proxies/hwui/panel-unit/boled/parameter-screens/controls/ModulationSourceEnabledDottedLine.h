#pragma once

#include "proxies/hwui/controls/DottedLine.h"
#include <sigc++/connection.h>

class Application;
class Parameter;

class ModulationSourceEnabledDottedLine : public DottedLine
{
 private:
  typedef DottedLine super;

 public:
  ModulationSourceEnabledDottedLine(const Rect& rect);
  virtual ~ModulationSourceEnabledDottedLine();

 private:
  ModulationSourceEnabledDottedLine(const ModulationSourceEnabledDottedLine& other);
  ModulationSourceEnabledDottedLine& operator=(const ModulationSourceEnabledDottedLine&);

  void onParameterSelected(Parameter* parameter, SignalOrigin signalType);
  void onParamValueChanged(const Parameter* param);
  bool redraw(FrameBuffer& fb);
  void setEnabled(bool e);

  sigc::connection m_paramValueConnection;
  bool m_enabled;
};
