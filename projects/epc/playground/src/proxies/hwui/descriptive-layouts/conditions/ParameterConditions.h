#pragma once

#include "ConditionBase.h"
#include <sigc++/connection.h>

class Parameter;

namespace DescriptiveLayouts
{
  namespace ParameterConditions
  {

    class ParameterCondition : public ConditionBase
    {
     public:
      ParameterCondition();
      virtual ~ParameterCondition();

     protected:
      void onParameterSelectionChanged(const Parameter* oldParam, Parameter* newParam, SignalOrigin signalType);
      virtual void onParameterChanged(const Parameter* param);

      sigc::connection m_paramChangedConnection;
      sigc::connection m_paramConnection;
    };

    class IsParameterModulateable : public ParameterCondition
    {
     public:
      bool check() const override;
    };

    class IsParameterUnmodulateable : public IsParameterModulateable
    {
     public:
      bool check() const override;
    };

    class HasNoMcSelected : public ParameterCondition
    {
     public:
      bool check() const override;
    };

    class HasMcSelected : public HasNoMcSelected
    {
     public:
      bool check() const override;
    };
  };
}
