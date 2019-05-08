#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>

class Parameter;
class ModulationRoutingParameter;
class Application;

class MiniModulationRouter : public ControlWithChildren
{
 private:
  typedef ControlWithChildren super;

 public:
  MiniModulationRouter(ModulationRoutingParameter *param, const Rect &rect);

 private:
  void onSourceParameterChanged(const Parameter *p);

  ModulationRoutingParameter *m_param = nullptr;
};
