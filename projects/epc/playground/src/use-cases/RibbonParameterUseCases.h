#pragma once

#include <glibmm/ustring.h>
#include <use-cases/PhysicalControlParameterUseCases.h>

class RibbonParameter;

class RibbonParameterUseCases : public PhysicalControlParameterUseCases
{
 public:
  explicit RibbonParameterUseCases(RibbonParameter* p);
  void setTouchBehaviour(const Glib::ustring& mode);
  void setReturnMode(const RibbonReturnMode& mode);
  void setReturnMode(const Glib::ustring& mode);

  void incTouchBehaviour();

 private:
  RibbonParameter* m_param;
};
