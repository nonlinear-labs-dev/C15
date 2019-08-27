#pragma once

#include <presets/EditBuffer.h>
#include <presets/PresetManager.h>
#include <Application.h>
#include <parameters/ModulateableParameter.h>
#include <tools/OnParameterChangedNotifier.h>
#include "ConditionBase.h"
#include <proxies/hwui/HWUI.h>

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
      void onParameterSelectionChanged(const Parameter* oldParam, Parameter* newParam);
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