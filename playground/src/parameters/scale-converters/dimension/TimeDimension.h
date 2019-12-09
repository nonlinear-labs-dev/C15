#pragma once

#include "Dimension.h"

template<size_t tPrecision>
class VariableTimeDimension : public Dimension
{
public:
  Glib::ustring getStingizerJS() const override
  {
    return "formatDimensionDigits(cpValue, 'ms', withUnit, " + std::to_string(tPrecision) + ")";
  }

  Glib::ustring stringize(const tDisplayValue &displayValue) const override
  {
    return formatDimensionDigits(displayValue, "ms", tPrecision);
  }

  static const VariableTimeDimension<tPrecision> &get()
  {
    static VariableTimeDimension<tPrecision> p;
    return p;
  }
};

class TimeDimension : public VariableTimeDimension<3>
{
};
