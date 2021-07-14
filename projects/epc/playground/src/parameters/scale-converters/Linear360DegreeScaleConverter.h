#pragma once

#include "LinearScaleConverter.h"

class Linear360DegreeScaleConverter : public LinearScaleConverter
{
 private:
  typedef LinearScaleConverter super;

 public:
  Linear360DegreeScaleConverter();
  ~Linear360DegreeScaleConverter() override;

  Linear360DegreeScaleConverter(const Linear360DegreeScaleConverter& other) = delete;
  Linear360DegreeScaleConverter& operator=(const Linear360DegreeScaleConverter&) = delete;
};
