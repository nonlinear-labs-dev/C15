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
  explicit ModulationSourceEnabledDottedLine(const Rect& rect);
  ~ModulationSourceEnabledDottedLine() override;

  ModulationSourceEnabledDottedLine(const ModulationSourceEnabledDottedLine& other) = delete;
  ModulationSourceEnabledDottedLine& operator=(const ModulationSourceEnabledDottedLine&) = delete;

 private:
  void onParameterSelected(Parameter* parameter);
  void onParamValueChanged(const Parameter* param);
  bool redraw(FrameBuffer& fb) override;
  void setEnabled(bool e);

  sigc::connection m_paramValueConnection;
  bool m_enabled;
};
