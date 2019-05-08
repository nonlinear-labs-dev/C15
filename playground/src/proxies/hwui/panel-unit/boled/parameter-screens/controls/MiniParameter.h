#pragma once

#include "proxies/hwui/controls/ControlWithChildren.h"

class Application;
class Parameter;
class MiniParameterLabel;

class MiniParameter : public ControlWithChildren
{
 private:
  typedef ControlWithChildren super;

 public:
  MiniParameter(Parameter* param, const Rect& pos);
  virtual ~MiniParameter();

  Parameter* getParameter();
  const Parameter* getParameter() const;
  void setSelected(bool selected);
  bool isSelected() const;

 private:
  // private methods
  MiniParameter(const MiniParameter& other);
  MiniParameter& operator=(const MiniParameter&);

  Parameter* m_param = nullptr;
  MiniParameterLabel* m_label = nullptr;
};
