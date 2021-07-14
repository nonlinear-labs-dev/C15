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
  ~MiniParameter() override;

  MiniParameter(const MiniParameter& other) = delete;
  MiniParameter& operator=(const MiniParameter&) = delete;

  Parameter* getParameter();
  const Parameter* getParameter() const;
  void setSelected(bool selected);
  bool isSelected() const;

 private:
  Parameter* m_param = nullptr;
  MiniParameterLabel* m_label = nullptr;
};
