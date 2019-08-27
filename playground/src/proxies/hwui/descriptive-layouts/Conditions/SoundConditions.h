#pragma once

#include <presets/EditBuffer.h>
#include <tools/EditBufferNotifier.h>
#include "ConditionBase.h"

namespace DescriptiveLayouts
{
  namespace SoundConditions
  {
    class EditBufferCondition : public ConditionBase
    {
    public:
      EditBufferCondition();
      virtual ~EditBufferCondition();
    protected:
      virtual void onEditBufferChanged();
      sigc::connection m_editbufferChangedConnection;
    };

    class IsSingleSound : public EditBufferCondition
    {
     public:
      bool check() const override;
    };

    class IsLayerSound : public EditBufferCondition
    {
     public:
      bool check() const override;
    };

    class IsSplitSound : public EditBufferCondition
    {
     public:
      bool check() const override;
    };
  };
}
