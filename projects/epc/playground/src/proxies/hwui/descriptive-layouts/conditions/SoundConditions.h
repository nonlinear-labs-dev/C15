#pragma once

#include "ConditionBase.h"
#include <sigc++/connection.h>

namespace DescriptiveLayouts
{
  namespace SoundConditions
  {
    class EditBufferCondition : public ConditionBase
    {
     public:
      EditBufferCondition();
      ~EditBufferCondition() override;

     protected:
      virtual void onEditBufferChanged();
      sigc::connection m_editbufferChangedConnection;
      sigc::connection m_editbufferTypeChangedConnection;
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
