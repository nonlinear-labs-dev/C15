#pragma once

#include <parameters/value/QuantizedValue.h>

class IncrementalChangerUseCases
{
 public:
  explicit IncrementalChangerUseCases(QuantizedValue::IncrementalChanger* changer);
  void changeBy(tControlPositionValue amount, bool fine);

 private:
  QuantizedValue::IncrementalChanger* m_changer;
};
