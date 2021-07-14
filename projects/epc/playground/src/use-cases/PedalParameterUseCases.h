#pragma once

#include <glibmm/ustring.h>
#include <use-cases/PhysicalControlParameterUseCases.h>

class PedalParameter;

class PedalParameterUseCases : public PhysicalControlParameterUseCases
{
 public:
  explicit PedalParameterUseCases(PedalParameter* p);

  void setPedalMode(const Glib::ustring& mode);

 private:
  PedalParameter* m_param;
};
