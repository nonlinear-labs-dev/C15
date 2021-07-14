#pragma once

#include "LinearScaleConverter.h"

class IdentityScaleConverter : public LinearScaleConverter
{
  typedef LinearScaleConverter super;

 public:
  IdentityScaleConverter();
  ~IdentityScaleConverter() override;
};
