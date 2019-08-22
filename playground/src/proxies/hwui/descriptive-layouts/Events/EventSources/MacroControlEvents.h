#pragma once
#include "base/EventSource.h"
#include "ParameterValueEvent.h"

namespace DescriptiveLayouts
{

  class CurrentMacroControlAssignment : public ParameterValueEvent<bool>
  {
   public:
    void onChange(const Parameter *p) override
    {
      if(auto modP = dynamic_cast<const ModulateableParameter *>(p))
      {
        auto v = modP->getModulationSource() != MacroControls::NONE;
        setValue(v);
      }
    }
  };

  class CurrentMacroControlSymbol : public ParameterValueEvent<DisplayString>
  {
   public:
    void onChange(const Parameter *p) override
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

  class CurrentMacroControlAmount : public ParameterValueEvent<DisplayString>
  {
    void onChange(const Parameter *p) override
    {
      if(const auto *modP = dynamic_cast<const ModulateableParameter *>(p))
      {
        setValue(DisplayString(modP->stringizeModulationAmount(), 0));
      }
    }
  };

  class CurrentMacroControlPositionText : public ParameterValueEvent<DisplayString>
  {
    void onChange(const Parameter *p) override
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
}