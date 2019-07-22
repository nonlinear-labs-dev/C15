#pragma once

#include <presets/EditBuffer.h>
#include <tools/EditBufferNotifier.h>
#include "ConditionBase.h"

namespace DescriptiveLayouts {
  namespace SoundConditions {
    class IsXSound : public ConditionBase
    {
    public:
      IsXSound();
      void onEditBufferChanged(const EditBuffer*);
    protected:
      OnEditBufferChangedNotifier<IsXSound> m_changed;
    };

    class IsSingleSound : public IsXSound
    {
    public:
      bool check() const override;
    };

    class IsLayerSound : public IsXSound
    {
    public:
      bool check() const override;
    };

    class IsSplitSound : public IsXSound
    {
    public:
      bool check() const override;
    };
  };
}
