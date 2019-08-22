#pragma once
#include "EditBufferEvent.h"

namespace DescriptiveLayouts
{
  class SoundEditHeading : public EditBufferEvent<DisplayString>
  {
   public:
    void onChange(const EditBuffer *eb) override
    {
      auto type = eb->getType();
      auto typeStr = toString(type);
      if(type == Type::Single)
        setValue({ typeStr, 0 });
      else
        setValue({ typeStr + (eb->isVGISelected() ? " [I]" : " [II]"), 0 });
    }
  };

  class SelectVGButtonText : public EditBufferEvent<DisplayString>
  {
   public:
    void onChange(const EditBuffer *eb) override
    {
      setValue({ eb->isVGISelected() ? "Select II" : "Select I", 0 });
    }
  };
}