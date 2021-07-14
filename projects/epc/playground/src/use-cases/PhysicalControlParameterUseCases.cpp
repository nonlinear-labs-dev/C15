#include "PhysicalControlParameterUseCases.h"

PhysicalControlParameterUseCases::PhysicalControlParameterUseCases(PhysicalControlParameter *p)
    : ParameterUseCases(p)
    , m_physicalParam { p }
{
}

void PhysicalControlParameterUseCases::changeFromPlaycontroller(double value)
{
  m_physicalParam->onChangeFromPlaycontroller(value);
}
