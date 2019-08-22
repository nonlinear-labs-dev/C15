#pragma once
#include "../base/EventSource.h"

namespace DescriptiveLayouts
{
  class BooleanTrue : public EventSource<bool>
  {
   public:
    BooleanTrue()
    {
      setValue(true);
    }
  };

  class BooleanFalse : public EventSource<bool>
  {
   public:
    BooleanFalse()
    {
      setValue(false);
    }
  };
}