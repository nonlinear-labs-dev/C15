#pragma once
#include "UnmodulateableMonoParameter.h"

class MonoPriorityParameter : public UnmodulateableMonoParameter
{
 public:
  MonoPriorityParameter(ParameterGroup* parent, const ParameterId& id);
};
