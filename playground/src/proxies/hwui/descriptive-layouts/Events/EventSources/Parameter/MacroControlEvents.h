#pragma once
#include "proxies/hwui/descriptive-layouts/Events/EventSources/base/EventSource.h"
#include "ParameterEvent.h"

namespace DescriptiveLayouts
{

  class CurrentMacroControlAssignment : public ParameterEvent<bool>
  {
   public:
    void onSelectedParameterChanged(const Parameter *p) override
    {
      if(auto modP = dynamic_cast<const ModulateableParameter *>(p))
      {
        auto v = modP->getModulationSource() != MacroControls::NONE;
        setValue(v);
      }
    }
  };

  class CurrentMacroControlSymbol : public ParameterEvent<DisplayString>
  {
   public:
    void onSelectedParameterChanged(const Parameter *p) override
    {
      if(const auto *modP = dynamic_cast<const ModulateableParameter *>(p))
      {
        if(auto mc = modP->getMacroControl())
        {
          setValue(DisplayString(mc->getShortName(), 0));
          return;
        }
      }
      setValue(DisplayString("[-]", 0));
    }
  };

  class CurrentMacroControlAmount : public ParameterEvent<DisplayString>
  {
    void onSelectedParameterChanged(const Parameter *p) override
    {
      if(const auto *modP = dynamic_cast<const ModulateableParameter *>(p))
      {
        setValue(DisplayString(modP->stringizeModulationAmount(), 0));
      }
    }
  };

  class CurrentMacroControlPositionText : public ParameterEvent<DisplayString>
  {
    void onSelectedParameterChanged(const Parameter *p) override
    {
      if(const auto *modP = dynamic_cast<const ModulateableParameter *>(p))
      {
        if(auto mc = modP->getMacroControl())
        {
          setValue(DisplayString(mc->getDisplayString(), 0));
        }
      }
    }
  };

  class CurrentModParamModRangeEventSource
      : public ParameterEvent<std::pair<tControlPositionValue, tControlPositionValue>>
  {
   public:
    void onSelectedParameterChanged(const Parameter *p) override
    {
      if(auto modP = dynamic_cast<const ModulateableParameter *>(p))
      {
        auto lower = std::min(modP->getModulationRange(true).first, modP->getModulationRange(true).second);
        lower = std::max(0., lower);
        auto upper = std::max(modP->getModulationRange(true).first, modP->getModulationRange(true).second);
        upper = std::min(1., upper);
        setValue(std::make_pair(lower, upper));
      }
    }
  };

  class CurrentMacroControlPosition : public ParameterEvent<tControlPositionValue>
  {
   public:
    void onSelectedParameterChanged(const Parameter *p) override
    {
      if(auto modP = dynamic_cast<const ModulateableParameter *>(p))
      {
        if(auto mc = modP->getMacroControl())
        {
          setValue(mc->getControlPositionValue());
          return;
        }
      }
    }
  };

  class MCPositionChanged : public ParameterEvent<bool>
  {
   public:
    void onSelectedParameterChanged(const Parameter *p) override
    {
      if(auto modP = dynamic_cast<const ModulateableParameter *>(p))
      {
        setValue(modP->isMacroControlAssignedAndChanged());
      }
    }
  };

  class MCSelectionChanged : public ParameterEvent<bool>
  {
  public:
    void onSelectedParameterChanged(const Parameter *p) override
    {
      if(auto modP = dynamic_cast<const ModulateableParameter *>(p))
      {
        setValue(modP->isModSourceChanged());
      }
    }
  };

  class MCAmountChanged : public ParameterEvent<bool>
  {
  public:
    void onSelectedParameterChanged(const Parameter *p) override
    {
      if(auto modP = dynamic_cast<const ModulateableParameter *>(p))
      {
        setValue(modP->isModAmountChanged());
      }
    }
  };
}