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
    class IsParameterModulateable : public ConditionBase
    {
     public:
      IsParameterModulateable();
      bool check() const override;
      void onParameterSelectionChanged(const Parameter *o, Parameter *n);

     protected:
      OnParameterSelectionChangedNotifier<IsParameterModulateable> m_selChanged;
    };

    class IsParameterUnmodulateable : public IsParameterModulateable
    {
     public:
      bool check() const override;
    };

    class HasNoMcSelected : public ConditionBase
    {
     public:
      HasNoMcSelected();
      bool check() const override;
      void onParameterChanged(const Parameter *parameter);

     protected:
      OnParameterChangedNotifier<HasNoMcSelected> m_paramSig;
    };

    class HasMcSelected : public HasNoMcSelected
    {
     public:
      bool check() const override;
    };
  };
}