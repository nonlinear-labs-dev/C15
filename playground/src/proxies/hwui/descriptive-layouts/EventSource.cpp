#include "EventSource.h"
#include "LayoutFactory.h"
#include <Application.h>
#include <proxies/hwui/HWUI.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/ModulateableParameter.h>
#include <groups/MacroControlsGroup.h>
#include <parameters/MacroControlParameter.h>

namespace DescriptiveLayouts
{
  using DisplayString = std::pair<Glib::ustring, int>;

  template <typename T> class EventSource : public EventSourceBase
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

    T m_lastValue{};
  };

  class GenericParameterDisplayValueEvent : public EventSource<DisplayString>
  {
   public:
    explicit GenericParameterDisplayValueEvent()
    {
      Application::get().getPresetManager()->getEditBuffer()->onChange(
          sigc::mem_fun(this, &GenericParameterDisplayValueEvent::onChange));
      Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
          sigc::mem_fun(this, &GenericParameterDisplayValueEvent::onParameterSelectionChanged));
    }

    virtual void onParameterChanged(const Parameter *p)
    {
      onChange();
    }

    virtual void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam)
    {
      onChange();
    }

    virtual void onChange() = 0;
  };

  class GenericRangeEventSource : public EventSource<std::pair<tControlPositionValue, tControlPositionValue>>
  {
   public:
    explicit GenericRangeEventSource()
    {
      Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
          sigc::mem_fun(this, &GenericRangeEventSource::onParameterSelectionChanged));
    }

   private:
    void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam)
    {
      m_connection.disconnect();
      if(newParam)
        m_connection
            = newParam->onParameterChanged(sigc::mem_fun(this, &GenericRangeEventSource::onParameterChanged), true);
    }

    virtual void onParameterChanged(const Parameter *p) = 0;

    sigc::connection m_connection;
  };

  class GenericValueEventSource : public EventSource<tControlPositionValue>
  {
   public:
    explicit GenericValueEventSource()
    {
      Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
          sigc::mem_fun(this, &GenericValueEventSource::onParameterSelectionChanged));
    }

   private:
    void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam)
    {
      m_connection.disconnect();
      if(newParam)
        m_connection
            = newParam->onParameterChanged(sigc::mem_fun(this, &GenericValueEventSource::onParameterChanged), true);
    }

    virtual void onParameterChanged(const Parameter *p) = 0;

    sigc::connection m_connection;
  };

  class ParameterGroupNameEventSource : public EventSource<DisplayString>
  {
   public:
    explicit ParameterGroupNameEventSource()
    {
      Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
          sigc::mem_fun(this, &ParameterGroupNameEventSource::onParameterSelectionChanged));
    }

   private:
    void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam)
    {
      setValue({ newParam ? newParam->getParentGroup()->getShortName() : "", 0 });
    }
  };

  class ParamIsBipolarEventSource : public EventSource<bool>
  {
   public:
    explicit ParamIsBipolarEventSource()
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

  class SliderRangeEventSource : public GenericRangeEventSource
  {
   public:
    explicit SliderRangeEventSource()
        : GenericRangeEventSource()
    {
    }

   private:
    virtual void onParameterChanged(const Parameter *p) override
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
  };

  class MCModRangeEventSource : public GenericRangeEventSource
  {
   public:
    explicit MCModRangeEventSource()
        : GenericRangeEventSource()
    {
    }

   private:
    void setRange(float from, float to)
    {
      setRangeOrdered(std::min(from, to), std::max(from, to));
    }

    void setRangeOrdered(float from, float to)
    {
      from = std::min(from, 1.0f);
      from = std::max(from, 0.0f);

      to = std::min(to, 1.0f);
      to = std::max(to, 0.0f);

      setValue(std::make_pair(from, to));
    }

    virtual void onParameterChanged(const Parameter *p) override
    {
      if(auto modP = dynamic_cast<const ModulateableParameter *>(p))
      {
        auto lower = std::min(modP->getModulationRange().first, modP->getModulationRange().second);
        lower = std::max(0., lower);
        auto upper = std::max(modP->getModulationRange().first, modP->getModulationRange().second);
        upper = std::min(1., upper);
        setValue(std::make_pair(lower, upper));
      }
    }
  };

  class CurrentMacroControlPosition : public GenericValueEventSource
  {
   public:
    explicit CurrentMacroControlPosition()
        : GenericValueEventSource()
    {
    }

   private:
    virtual void onParameterChanged(const Parameter *p) override
    {
      if(auto modP = dynamic_cast<const ModulateableParameter *>(p))
      {
        if(auto mc = modP->getMacroControl())
        {
          setValue(mc->getControlPositionValue());
        }
      }
    }
  };

  class ParameterNameEventSource : public EventSource<DisplayString>
  {
   public:
    explicit ParameterNameEventSource()
    {
      Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
          sigc::mem_fun(this, &ParameterNameEventSource::onParameterSelectionChanged));
    }

   private:
    void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam)
    {
      if(newParam)
        setValue({ newParam->getLongName(), 0 });
    }
  };

  class ParameterDisplayStringEventSource : public EventSource<DisplayString>
  {
   public:
    explicit ParameterDisplayStringEventSource()
    {
      Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
          sigc::mem_fun(this, &ParameterDisplayStringEventSource::onParameterSelectionChanged));

      Application::get().getHWUI()->onModifiersChanged(
          sigc::mem_fun(this, &ParameterDisplayStringEventSource::onModifierChanged));
    }

   private:
    void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam)
    {
      m_connection.disconnect();

      if(newParam)
        m_connection = newParam->onParameterChanged(
            sigc::mem_fun(this, &ParameterDisplayStringEventSource::onParameterChanged), true);
    }

    void onModifierChanged(::ButtonModifiers mods)
    {
      onParameterChanged(Application::get().getPresetManager()->getEditBuffer()->getSelectedParameter());
    }

    void onParameterChanged(const Parameter *p)
    {
      auto str = p->getDisplayString();

      if(Application::get().getHWUI()->isModifierSet(ButtonModifier::FINE))
      {
        setValue({ str + " F", 2 });
      }
      else
      {
        setValue({ str, 0 });
      }
    }

    sigc::connection m_connection;
  };

  class CurrentParameterGroupLockStatus : public EventSource<bool>
  {
   public:
    explicit CurrentParameterGroupLockStatus()
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
      setValue(Application::get().getPresetManager()->getEditBuffer()->getSelectedParameter()->isLocked());
    }
  };

  class CurrentMacroControlAsignment : public EventSource<bool>
  {
   public:
    explicit CurrentMacroControlAsignment()
    {
      Application::get().getPresetManager()->getEditBuffer()->onChange(
          sigc::mem_fun(this, &CurrentMacroControlAsignment::onChange));
      Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
          sigc::mem_fun(this, &CurrentMacroControlAsignment::onParameterSelectionChanged));
    }

   private:
    void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam)
    {
      onChange();
    }

    void onChange()
    {
      if(auto modParam = dynamic_cast<ModulateableParameter *>(
             Application::get().getPresetManager()->getEditBuffer()->getSelectedParameter()))
      {
        setValue(modParam->getModulationSource() != ModulateableParameter::NONE);
        return;
      }
      setValue(false);
    }
  };

  class CurrentMacroControlSymbol : public EventSource<DisplayString>
  {
   public:
    explicit CurrentMacroControlSymbol()
    {
      Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
          sigc::mem_fun(this, &CurrentMacroControlSymbol::onParameterSelectionChanged));
    }

   private:
    sigc::connection m_paramValueConnection;

    void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam)
    {
      if(newParam)
      {
        m_paramValueConnection.disconnect();
        m_paramValueConnection
            = newParam->onParameterChanged(sigc::mem_fun(this, &CurrentMacroControlSymbol::onParamValueChanged));
      }
    }

    void onParamValueChanged(const Parameter *param)
    {
      if(const auto *modP = dynamic_cast<const ModulateableParameter *>(param))
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

  class CurrentMacroControlAmount : public GenericParameterDisplayValueEvent
  {
    virtual void onChange() override
    {
      auto eb = Application::get().getPresetManager()->getEditBuffer();
      if(const ModulateableParameter *modP = dynamic_cast<const ModulateableParameter *>(eb->getSelectedParameter()))
      {
        setValue(DisplayString(modP->getModAmountAsDisplayValue(), 0));
      }
    }
  };

  class CurrentMacroControlPositionText : public GenericParameterDisplayValueEvent
  {
    virtual void onChange() override
    {
      auto eb = Application::get().getPresetManager()->getEditBuffer();
      if(const ModulateableParameter *modP = dynamic_cast<const ModulateableParameter *>(eb->getSelectedParameter()))
      {
        if(auto mc = modP->getMacroControl())
        {
          setValue(DisplayString(mc->getDisplayString(), 0));
        }
      }
    }
  };

  EventSourceBroker &EventSourceBroker::get()
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
    m_map[EventSources::MacroControlAsignment] = std::make_unique<CurrentMacroControlAsignment>();
    m_map[EventSources::MacroControlAmount] = std::make_unique<CurrentMacroControlAmount>();
    m_map[EventSources::MacroControlPosition] = std::make_unique<CurrentMacroControlPosition>();
    m_map[EventSources::MacroControlPositionText] = std::make_unique<CurrentMacroControlPositionText>();
    m_map[EventSources::MCModRange] = std::make_unique<MCModRangeEventSource>();
  }

  sigc::connection EventSourceBroker::connect(EventSources source, std::function<void(std::any)> cb)
  {
    if(source == EventSources::None)
      return {};

    return m_map.at(source)->connect(cb);
  }
}
