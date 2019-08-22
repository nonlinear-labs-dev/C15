#pragma once
#include "EditBufferEvent.h"

namespace DescriptiveLayouts
{
  class EditBufferTypeStringEvent : public EditBufferEvent<DisplayString>
  {
   public:
    void onChange(const EditBuffer *eb) override
    {
      const auto type = eb->getType();
      const auto typeStr = toString(type);

      if(type == Type::Single)
      {
        setValue({ typeStr, 0 });
      }
      else
      {
        const auto suffix = eb->m_vgISelected ? "  I" : "  II";
        const auto str = typeStr + suffix;
        setValue({ str, 0 });
      }
    }
  };

  class EditBufferName : public EditBufferEvent<DisplayString>
  {
   public:
    void onChange(const EditBuffer *eb) override
    {
      setValue({ eb->getName(), 0 });
    }
  };

  class CurrentVoiceGroupName : public EditBufferEvent<DisplayString>
  {
   public:
    void onChange(const EditBuffer *eb) override
    {
      setValue({ eb->getCurrentVoiceGroupName(), 0 });
    }
  };

  class IsCurrentVGI : public EditBufferEvent<bool>
  {
   public:
    void onChange(const EditBuffer *eb) override
    {
      setValue(eb->isVGISelected());
    }
  };

  class IsCurrentVGII : public EditBufferEvent<bool>
  {
   public:
    void onChange(const EditBuffer *eb) override
    {
      setValue(eb->isVGIISelected());
    }
  };
}