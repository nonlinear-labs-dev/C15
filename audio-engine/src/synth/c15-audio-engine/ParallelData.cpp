#include "ParallelData.h"

class Test
{
 public:
  Test()
  {
    float data[128];
    ParallelData<DataMode::Owned, float, 128> owned;
    ParallelData<DataMode::Pointer, float, 128> shared(data);
    auto res = owned * shared;
    res -= owned;
    res += owned - shared;
  }
};
