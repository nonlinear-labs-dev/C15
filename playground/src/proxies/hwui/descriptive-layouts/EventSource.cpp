#include "EventSource.h"
#include "LayoutFactory.h"
#include <Application.h>
#include <proxies/hwui/HWUI.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/ModulateableParameter.h>
#include <groups/MacroControlsGroup.h>

namespace DescriptiveLayouts
{
  using DisplayString = std::pair<Glib::ustring, int>;

  template<typename T>
    class EventSource : public EventSourceBase
    {
      protected:
        void setValue(const T &v)
        {
          if(v != m_lastValue)
          {
            m_lastValue = v;
            m_outputSignal.send(m_lastValue);
          }
        }

      private:
        std::any getLastValue() const override
        {
          return m_lastValue;
        }

        T m_lastValue { };
    };

  class ParameterGroupNameEventSource : public EventSource<DisplayString>
  {
    public:
      ParameterGroupNameEventSource()
      {
        Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
            sigc::mem_fun(this, &ParameterGroupNameEventSource::onParameterSelectionChanged));
      }

    private:
      void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam)
      {
        setValue( { newParam ? newParam->getParentGroup()->getShortName() : "", 0 });
      }
  };

  class ParamIsBipolarEventSource : public EventSource<bool>
  {
    public:
      ParamIsBipolarEventSource()
      {
        Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
            sigc::mem_fun(this, &ParamIsBipolarEventSource::onParameterSelectionChanged));
      }

    private:
      void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam)
      {
        if(newParam)
        {
          setValue(newParam->isBiPolar());
        }
      }
  };

  class SliderRangeEventSource : public EventSource<std::pair<tControlPositionValue, tControlPositionValue>>
  {
    public:
      SliderRangeEventSource()
      {
        Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
            sigc::mem_fun(this, &SliderRangeEventSource::onParameterSelectionChanged));
      }

    private:
      void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam)
      {
        m_connection.disconnect();
        if(newParam)
          m_connection = newParam->onParameterChanged(sigc::mem_fun(this, &SliderRangeEventSource::onParameterChanged), true);
      }

      void onParameterChanged(const Parameter* p)
      {
        auto v = p->getControlPositionValue();

        if(p->isBiPolar())
        {
          auto value = (v + 1) / 2;
          auto width = value - 0.5;

          setValue(std::make_pair(0.5, width));
        }
        else
        {
          setValue(std::make_pair(0, v));
        }
      }

      sigc::connection m_connection;
  };

  class ParameterNameEventSource : public EventSource<DisplayString>
  {
    public:
      ParameterNameEventSource()
      {
        Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
            sigc::mem_fun(this, &ParameterNameEventSource::onParameterSelectionChanged));
      }

    private:
      void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam)
      {
        if(newParam)
          setValue( { newParam->getLongName(), 0 });
      }
  };

  class ParameterDisplayStringEventSource : public EventSource<DisplayString>
  {
    public:
      ParameterDisplayStringEventSource()
      {
        Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
            sigc::mem_fun(this, &ParameterDisplayStringEventSource::onParameterSelectionChanged));

        Application::get().getHWUI()->onModifiersChanged(sigc::mem_fun(this, &ParameterDisplayStringEventSource::onModifierChanged));
      }

    private:
      void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam)
      {
        m_connection.disconnect();

        if(newParam)
          m_connection = newParam->onParameterChanged(sigc::mem_fun(this, &ParameterDisplayStringEventSource::onParameterChanged), true);
      }

      void onModifierChanged(::ButtonModifiers mods)
      {
        onParameterChanged(Application::get().getPresetManager()->getEditBuffer()->getSelected());
      }

      void onParameterChanged(const Parameter* p)
      {
        auto str = p->getDisplayString();

        if(Application::get().getHWUI()->isModifierSet(ButtonModifier::FINE))
        {
          setValue( { str + " F", 2 });
        }
        else
        {
          setValue( { str, 0 });
        }
      }

      sigc::connection m_connection;
  };

    class CurrentParameterGroupLockStatus : public EventSource<bool> {
    public:
        CurrentParameterGroupLockStatus()
        {
          Application::get().getPresetManager()->getEditBuffer()->onLocksChanged(
                  sigc::mem_fun(this, &CurrentParameterGroupLockStatus::onLockChanged));

          Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
                  sigc::mem_fun(this, &CurrentParameterGroupLockStatus::onParameterSelectionChanged));
        }

    private:
        void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam)
        {
            onLockChanged();
        }

        void onLockChanged()
        {
            setValue(Application::get().getPresetManager()->getEditBuffer()->getSelected()->isLocked());
        }
    };

    class CurrentMacroControlSymbol : public EventSource<DisplayString> {
    public:
        CurrentMacroControlSymbol()
        {
          Application::get ().getPresetManager ()->getEditBuffer ()->onSelectionChanged (sigc::mem_fun (this, &CurrentMacroControlSymbol::onParameterSelectionChanged));
        }

    private:
        sigc::connection m_paramValueConnection;

        void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam)
        {
          if (newParam)
          {
            m_paramValueConnection.disconnect ();
            m_paramValueConnection = newParam->onParameterChanged (sigc::mem_fun (this, &CurrentMacroControlSymbol::onParamValueChanged));
          }
        }

        void onParamValueChanged (const Parameter* param)
        {
          if (const auto *modP = dynamic_cast<const ModulateableParameter*> (param))
          {
            uint16_t id = MacroControlsGroup::modSrcToParamID (modP->getModulationSource ());

            if (auto mc = Application::get ().getPresetManager ()->getEditBuffer ()->findParameterByID (id))
            {
              DisplayString s(mc->getShortName(), 0);
              setValue(s);
              return;
            }
          }
          DisplayString s("[-]", 0);
          setValue(s);
        }
    };
  EventSourceBroker& EventSourceBroker::get()
  {
    static EventSourceBroker s;
    return s;
  }

  EventSourceBroker::EventSourceBroker()
  {
    m_map[EventSources::ParameterGroupName] = std::make_unique<ParameterGroupNameEventSource>();
    m_map[EventSources::SliderRange] = std::make_unique<SliderRangeEventSource>();
    m_map[EventSources::IsBipolar] = std::make_unique<ParamIsBipolarEventSource>();
    m_map[EventSources::ParameterName] = std::make_unique<ParameterNameEventSource>();
    m_map[EventSources::ParameterDisplayString] = std::make_unique<ParameterDisplayStringEventSource>();
    m_map[EventSources::LockStatus] = std::make_unique<CurrentParameterGroupLockStatus>();
    m_map[EventSources::MacroControlSymbol] = std::make_unique<CurrentMacroControlSymbol>();
  }

  sigc::connection EventSourceBroker::connect(EventSources source, std::function<void(std::any)> cb)
  {
    if(source == EventSources::None)
      return
      {};

    return m_map.at(source)->connect(cb);
  }
}
