#pragma once
#include "ArrowIncrementDecrementOverlay.h"

class Parameter;
class ParameterValueLabel;

class ParameterOverlay : public ArrowIncrementDecrementOverlay
{
 public:
  explicit ParameterOverlay(const Rect& rect, Parameter* parameter);
  void onLeft(bool down) override;
  void onRight(bool down) override;

 private:
  ParameterValueLabel* m_label;
  Parameter* const m_parameter;
};