#pragma once
#include "ParameterEvent.h"

namespace DescriptiveLayouts
{
  class ParameterGroupNameEventSource : public ParameterEvent<DisplayString>
  {
   public:
    void onSelectedParameterChanged(const Parameter *p) override
    {
      setValue({ p ? p->getParentGroup()->getShortName() : "", 0 });
    }
  };

  class ParameterIsBipolarEventSource : public ParameterEvent<bool>
  {
   public:
    void onSelectedParameterChanged(const Parameter *p) override
    {
      setValue(p ? p->isBiPolar() : false);
    }
  };

  class CurrentParameterControlPosition : public ParameterEvent<tControlPositionValue>
  {
   public:
    void onSelectedParameterChanged(const Parameter *p) override
    {
      if(p)
      {
        setValue(p->getControlPositionValue());
      }
    }
  };

  class ParameterNameEventSource : public ParameterEvent<DisplayString>
  {
    void onSelectedParameterChanged(const Parameter *p) override
    {
      if(p)
        setValue({ p->getLongName(), 0 });
    }
  };

  class ParameterNameWithStateSuffixEventSource : public ParameterEvent<DisplayString>
  {
   public:
    void onSelectedParameterChanged(const Parameter *parameter) override
    {
      if(parameter)
      {
        auto changed = parameter->isChangedFromLoaded();
        auto displayStr = parameter->getLongName().append(changed ? "*" : "");
        setValue(DisplayString{ displayStr, changed ? 1 : 0 });
      }
      else
      {
        setValue(DisplayString{ "", 0 });
      }
    }
  };

  class ParameterDisplayStringEventSource : public ParameterEvent<DisplayString>
  {
   public:
    explicit ParameterDisplayStringEventSource()
    {
      m_modifierConnection = Application::get().getHWUI()->onModifiersChanged(
          sigc::mem_fun(this, &ParameterDisplayStringEventSource::onModifierChanged));
    }

    ~ParameterDisplayStringEventSource()
    {
      m_modifierConnection.disconnect();
    }

    void onSelectedParameterChanged(const Parameter *p) override
    {
      auto str = p ? p->getDisplayString() : Glib::ustring{};

      if(Application::get().getHWUI()->isModifierSet(ButtonModifier::FINE))
      {
        setValue({ str + " F", 2 });
      }
      else
      {
        setValue({ str, 0 });
      }
    }

   private:
    void onModifierChanged(::ButtonModifiers mods)
    {
      onSelectedParameterChanged(Application::get().getPresetManager()->getEditBuffer()->getSelected());
    }

   private:
    sigc::connection m_modifierConnection;
  };

  class ParameterValueChanged : public ParameterEvent<bool>
  {
   public:
    void onSelectedParameterChanged(const Parameter *p) override
    {
      setValue(p && p->isValueChangedFromLoaded());
    }
  };
}