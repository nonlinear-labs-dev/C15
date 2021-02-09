#pragma once
#include "../parameter/ParameterEvent.h"

namespace DescriptiveLayouts
{
  class IsOnlyParameterOnButton : public ParameterEvent<bool>
  {
   public:
    IsOnlyParameterOnButton()
        : ParameterEvent<bool>()
    {
      m_onParameterChangedConnection.disconnect();
    }

    void onSelectedParameterChanged(const Parameter *p) override
    {
    }

    void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam, SignalOrigin signalType) override
    {
      if(newParam)
      {
        auto button = m_mapping.findButton(newParam->getID().getNumber());
        forwardValue(m_mapping.findParameters(button).size() == 1);
      }
      else
      {
        setValue(false);
      }
    }

   protected:
    virtual void forwardValue(bool value)
    {
      setValue(value);
    }

    ButtonParameterMapping m_mapping{};
  };

  class IsNotOnlyParameterOnButton : public IsOnlyParameterOnButton
  {
   protected:
    void forwardValue(bool value) override
    {
      setValue(!value);
    }
  };
}
