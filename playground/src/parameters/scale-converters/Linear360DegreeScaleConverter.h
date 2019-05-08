#pragma once

#include "LinearScaleConverter.h"

class Linear360DegreeScaleConverter : public LinearScaleConverter
{
 private:
  typedef LinearScaleConverter super;

 public:
  Linear360DegreeScaleConverter();
  virtual ~Linear360DegreeScaleConverter();

 private:
  Linear360DegreeScaleConverter(const Linear360DegreeScaleConverter& other);
  Linear360DegreeScaleConverter& operator=(const Linear360DegreeScaleConverter&);
};
