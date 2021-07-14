#pragma once

#include "ParameterEvent.h"

class Parameter;

namespace DescriptiveLayouts
{
  class CurrentParameterGroupLockStatus : public ParameterEvent<bool>
  {
   public:
    explicit CurrentParameterGroupLockStatus();
    ~CurrentParameterGroupLockStatus() override;

    void onSelectedParameterChanged(const Parameter *p) override;

   private:
    void onLockChanged();

   protected:
    sigc::connection m_locksConnection;
  };
}
