#pragma once
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationCarousel.h>

namespace DescriptiveLayouts
{

  struct UICache
  {

    static UICache& get()
    {
      static UICache cache;
      return cache;
    }

  };
}