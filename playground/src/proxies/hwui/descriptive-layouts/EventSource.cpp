#include <utility>

#include "EventSource.h"
#include "LayoutFactory.h"
#include <Application.h>
#include <proxies/hwui/HWUI.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/ModulateableParameter.h>
#include <groups/MacroControlsGroup.h>
#include <parameters/MacroControlParameter.h>
#include <tools/OnParameterChangedNotifier.h>
#include <proxies/hwui/panel-unit/ButtonParameterMapping.h>
#include <presets/Bank.h>
#include <presets/Preset.h>
#include <tools/EditBufferNotifier.h>
#include <tools/SingeltonShortcuts.h>
#include <device-settings/AutoLoadSelectedPreset.h>

namespace DescriptiveLayouts
{
  using DisplayString = std::pair<Glib::ustring, int>;

  template <typename T> class EventSource : public EventSourceBase
  {
   public:
    virtual void setValue(const T &v)
    {
      set_as(v);
    }

   private:
    template <class TT> void set_as(const TT &v)
    {
      if(v != m_lastValue)
      {
        m_lastValue = v;
        m_outputSignal.send(m_lastValue);
      }
    }

    void set_as(const DisplayString &v)
    {
      auto hasher = std::hash<std::string>();
      const auto hashNew = hasher(v.first);
      const auto hashOld = hasher(m_lastValue.first);

      if(v.first != m_lastValue.first || hashOld != hashNew)
      {
        m_lastValue = v;
        m_outputSignal.send(m_lastValue);
      }
    }

   protected:
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
        : m_notifier(this)
    {
    }

    virtual void onParameterChanged(const Parameter *p) = 0;

   protected:
    OnParameterChangedNotifier<GenericRangeEventSource> m_notifier;
  };

  class GenericValueEventSource : public EventSource<tControlPositionValue>
  {
   public:
    explicit GenericValueEventSource()
        : m_notifier(this)
    {
    }

    virtual void onParameterChanged(const Parameter *p) = 0;

   private:
    OnParameterChangedNotifier<GenericValueEventSource> m_notifier;
  };

  class ParameterGroupNameEventSource : public EventSource<DisplayString>
  {
   public:
    explicit ParameterGroupNameEventSource()
        : m_notifier(this)
    {
    }

    void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam)
    {
      setValue({ newParam ? newParam->getParentGroup()->getShortName() : "", 0 });
    }

   private:
    OnParameterSelectionChangedNotifier<ParameterGroupNameEventSource> m_notifier;
  };

  class ParamIsBipolarEventSource : public EventSource<bool>
  {
   public:
    explicit ParamIsBipolarEventSource()
        : m_notifier(this)
    {
    }

    void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam)
    {
      setValue(newParam ? newParam->isBiPolar() : false);
    }

   private:
    OnParameterSelectionChangedNotifier<ParamIsBipolarEventSource> m_notifier;
  };

  class SliderRangeEventSource : public GenericRangeEventSource
  {
   public:
    explicit SliderRangeEventSource()
        : GenericRangeEventSource()
    {
    }

    void onParameterChanged(const Parameter *p) override
    {
      auto v = p->getControlPositionValue();
      if(p->isBiPolar())
      {
        auto value = (v + 1) / 2;
        setValue(std::make_pair(0.5, value));
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
    void setRangeOrdered(float from, float to)
    {
      from = std::min(from, 1.0f);
      from = std::max(from, 0.0f);

      to = std::min(to, 1.0f);
      to = std::max(to, 0.0f);

      setValue(std::make_pair(from, to));
    }

   public:
    void onParameterChanged(const Parameter *p) override
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

  class CurrentMacroControlPosition : public EventSource<tControlPositionValue>
  {
   public:
    explicit CurrentMacroControlPosition()
        : EventSource()
        , m_notifier{ this }
    {
    }

    void onModulationSourceChanged(const ModulateableParameter *modP)
    {
      if(modP)
      {
        if(auto mc = modP->getMacroControl())
        {
          setValue(mc->getControlPositionValue());
        }
      }
    }

   protected:
    OnModulationChangedNotifier<CurrentMacroControlPosition> m_notifier;
  };

  class CurrentParameterControlPosition : public GenericValueEventSource
  {
   public:
    explicit CurrentParameterControlPosition()
        : GenericValueEventSource()
    {
    }

   private:
    void onParameterChanged(const Parameter *p) override
    {
      if(p)
      {
        setValue(p->getControlPositionValue());
      }
    }
  };

  class ParameterNameEventSource : public EventSource<DisplayString>
  {
   public:
    explicit ParameterNameEventSource()
    {
    }

   private:
    void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam)
    {
      if(newParam)
        setValue({ newParam->getLongName(), 0 });
    }
  };

  class ParameterNameWithStateSuffixEventSource : public EventSource<DisplayString>
  {
   public:
    explicit ParameterNameWithStateSuffixEventSource()
        : m_notifier(this)
    {
    }

    void onParameterChanged(const Parameter *parameter)
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

   protected:
    OnParameterChangedNotifier<ParameterNameWithStateSuffixEventSource> m_notifier;
  };

  class ParameterDisplayStringEventSource : public EventSource<DisplayString>
  {
   public:
    explicit ParameterDisplayStringEventSource()
        : m_notifier(this)
    {

      Application::get().getHWUI()->onModifiersChanged(
          sigc::mem_fun(this, &ParameterDisplayStringEventSource::onModifierChanged));
    }

   private:
    void onModifierChanged(::ButtonModifiers mods)
    {
      onParameterChanged(Application::get().getPresetManager()->getEditBuffer()->getSelected());
    }

   public:
    void onParameterChanged(const Parameter *p)
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
    OnParameterChangedNotifier<ParameterDisplayStringEventSource> m_notifier;
    sigc::connection m_connection;
  };

  class CurrentParameterGroupLockStatus : public EventSource<bool>
  {
   public:
    explicit CurrentParameterGroupLockStatus()
        : m_notifier(this)
    {
      Application::get().getPresetManager()->getEditBuffer()->onLocksChanged(
          sigc::mem_fun(this, &CurrentParameterGroupLockStatus::onLockChanged));
    }

    void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam)
    {
      onLockChanged();
    }

   private:
    void onLockChanged()
    {
      auto param = Application::get().getPresetManager()->getEditBuffer()->getSelected();
      setValue(param ? param->isLocked() : false);
    }

    OnParameterSelectionChangedNotifier<CurrentParameterGroupLockStatus> m_notifier;
  };

  class CurrentMacroControlAsignment : public EventSource<bool>
  {
   public:
    CurrentMacroControlAsignment()
        : m_changedNotifier{ this }
    {
      onParameterChanged(SiSc::EB::getCurrentParameter());
    }

    void onParameterChanged(const Parameter *p)
    {
      if(auto modP = dynamic_cast<const ModulateableParameter *>(p))
      {
        auto v = modP->getModulationSource() != MacroControls::NONE;
        setValue(v);
      }
    }

   protected:
    OnParameterChangedNotifier<CurrentMacroControlAsignment> m_changedNotifier;
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
    void onChange() override
    {
      auto eb = Application::get().getPresetManager()->getEditBuffer();
      if(const auto *modP = dynamic_cast<const ModulateableParameter *>(eb->getSelected()))
      {
        setValue(DisplayString(modP->stringizeModulationAmount(), 0));
      }
    }
  };

  class CurrentMacroControlPositionText : public GenericParameterDisplayValueEvent
  {
    void onChange() override
    {
      auto eb = Application::get().getPresetManager()->getEditBuffer();
      if(const auto *modP = dynamic_cast<const ModulateableParameter *>(eb->getSelected()))
      {
        if(auto mc = modP->getMacroControl())
        {
          setValue(DisplayString(mc->getDisplayString(), 0));
        }
      }
    }
  };

  class SoundHeaderText : public EventSource<DisplayString>
  {
   public:
    SoundHeaderText()
        : m_not{ this }
    {
    }
    void onEditBufferChanged(const EditBuffer *e)
    {
      setValue(std::any_cast<DisplayString>(getLastValue()));
    }

   protected:
    std::any getLastValue() const override
    {
      auto editBuffer = Application::get().getPresetManager()->getEditBuffer();
      auto type = editBuffer->getType();
      if(type == Type::Single)
        return DisplayString{ toString(type), 0 };
      return DisplayString{ toString(type) + (editBuffer->m_vgISelected ? "  I" : "  II"), 0 };
    }
    OnEditBufferChangedNotifier<SoundHeaderText> m_not;
  };

  class EditBufferName : public EventSource<DisplayString>
  {
   protected:
    std::any getLastValue() const override
    {
      auto name = Application::get().getPresetManager()->getEditBuffer()->getName();
      return DisplayString{ name, 0 };
    }
  };

  class CurrentVoiceGroupName : public EventSource<DisplayString>
  {
   protected:
    std::any getLastValue() const override
    {
      auto name = Application::get().getPresetManager()->getEditBuffer()->getCurrentVoiceGroupName();
      return DisplayString{ name, 0 };
    }
  };

  class BooleanTrue : public EventSource<bool>
  {
   protected:
    std::any getLastValue() const override
    {
      return std::any(true);
    }
  };

  class BooleanFalse : public EventSource<bool>
  {
   protected:
    std::any getLastValue() const override
    {
      return std::any(false);
    }
  };

  class IsOnlyParameterOnButton : public EventSource<bool>
  {
   public:
    IsOnlyParameterOnButton()
        : m_notifier(this)
    {
    }

    void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam)
    {
      if(newParam)
      {
        auto button = m_mapping.findButton(newParam->getID());
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
    OnParameterSelectionChangedNotifier<IsOnlyParameterOnButton> m_notifier;
  };

  class IsNotOnlyParameterOnButton : public IsOnlyParameterOnButton
  {
   protected:
    void forwardValue(bool value) override
    {
      setValue(!value);
    }
  };

  class DirectLoadStatus : public EventSource<bool>
  {
   public:
    DirectLoadStatus()
    {
      Application::get().getSettings()->getSetting<AutoLoadSelectedPreset>()->onChange([&](const Setting *s) {
        if(auto ss = dynamic_cast<const AutoLoadSelectedPreset *>(s))
        {
          setValue(ss->get());
        }
      });
    }
  };

  class StaticText : public EventSource<DisplayString>
  {
   public:
    explicit StaticText(Glib::ustring string)
        : EventSource()
        , m_text{ std::move(string) }
    {
    }

   protected:
    Glib::ustring m_text;
    std::any getLastValue() const override
    {
      return DisplayString{ m_text, 0 };
    }
  };

  class MCPositionChanged : public EventSource<bool>
  {
   public:
    MCPositionChanged()
        : m_changedNotifier{ this }
    {
    }

    void onParameterChanged(const Parameter *p)
    {
      if(auto modP = dynamic_cast<const ModulateableParameter *>(p))
      {
        auto v = modP->isMacroControlAssignedAndChanged();
        setValue(v);
      }
    }

   protected:
    OnParameterChangedNotifier<MCPositionChanged> m_changedNotifier;
  };

  class MCSelectionChanged : public EventSource<bool>
  {
   public:
    MCSelectionChanged()
        : m_changedNotifier{ this }
    {
    }

    void onParameterChanged(const Parameter *p)
    {
      if(auto modP = dynamic_cast<const ModulateableParameter *>(p))
      {
        auto v = modP->isModSourceChanged();
        setValue(v);
      }
    }

   protected:
    OnParameterChangedNotifier<MCSelectionChanged> m_changedNotifier;
  };

  class MCAmountChanged : public EventSource<bool>
  {
   public:
    MCAmountChanged()
        : m_changedNotifier{ this }
    {
    }

    void onParameterChanged(const Parameter *p)
    {
      if(auto modP = dynamic_cast<const ModulateableParameter *>(p))
      {
        auto v = modP->isModAmountChanged();
        setValue(v);
      }
    }

   protected:
    OnParameterChangedNotifier<MCAmountChanged> m_changedNotifier;
  };

  class ParameterValueChanged : public EventSource<bool>
  {
   public:
    ParameterValueChanged()
        : m_paramChanged{ this }
    {
      auto param = SiSc::EB::getCurrentParameter();
      onParameterChanged(param);
    }

    void onParameterChanged(const Parameter *p)
    {
      if(p)
        setValue(p->isValueChangedFromLoaded());
      else
        setValue(false);
    }

   protected:
    std::any getLastValue() const override
    {
      return std::any(SiSc::EB::getCurrentParameter()->isValueChangedFromLoaded());
    }

    OnParameterChangedNotifier<ParameterValueChanged> m_paramChanged;
  };

  class FullSoundName : public EventSource<DisplayString>
  {
   protected:
    std::any getLastValue() const override
    {
      auto pm = Application::get().getPresetManager();
      auto name = pm->getEditBuffer()->getName();

      if(auto bank = pm->findBankWithPreset(pm->getEditBuffer()->getUUIDOfLastLoadedPreset()))
      {
        auto bankNum = pm->getBankPosition(bank->getUuid());
        if(auto preset = bank->getSelectedPreset())
        {
          auto num = bank->getPresetPosition(preset->getUuid());
          auto changed = pm->getEditBuffer()->anyParameterChanged();
          return DisplayString{ std::to_string(bankNum) + "-" + std::to_string(num) + (changed ? "*" : "") + name, 0 };
        }
      }
      return DisplayString{ name, 0 };
    }
  };

  class IsCurrentVGI : public EventSource<bool>
  {
   public:
    IsCurrentVGI()
        : m_changed(this)
    {
    }
    void onEditBufferChanged(const EditBuffer *eb)
    {
      setValue(std::any_cast<bool>(getLastValue()));
    }

   protected:
    std::any getLastValue() const override
    {
      return Application::get().getPresetManager()->getEditBuffer()->isVGISelected();
    }
    OnEditBufferChangedNotifier<IsCurrentVGI> m_changed;
  };

  class IsCurrentVGII : public EventSource<bool>
  {
   public:
    IsCurrentVGII()
        : m_changed(this)
    {
    }
    void onEditBufferChanged(const EditBuffer *eb)
    {
      setValue(std::any_cast<bool>(getLastValue()));
    }

   protected:
    std::any getLastValue() const override
    {
      return Application::get().getPresetManager()->getEditBuffer()->isVGIISelected();
    }
    OnEditBufferChangedNotifier<IsCurrentVGII> m_changed;
  };

  class SoundEditHeading : public EventSource<DisplayString>
  {
   public:
    SoundEditHeading()
        : m_changed(this)
    {
    }
    void onEditBufferChanged(const EditBuffer *eb)
    {
      setValue(std::any_cast<DisplayString>(getLastValue()));
    }

   protected:
    std::any getLastValue() const override
    {
      auto eb = Application::get().getPresetManager()->getEditBuffer();
      auto type = eb->getType();

      if(type == Type::Single)
        return DisplayString({ toString(type), 0 });

      return DisplayString({ toString(type) + (eb->isVGISelected() ? " [I]" : " [II]"), 0 });
    }
    OnEditBufferChangedNotifier<SoundEditHeading> m_changed;
  };

  class SelectVGButtonText : public EventSource<DisplayString>
  {
   public:
    SelectVGButtonText()
        : m_changed(this)
    {
    }
    void onEditBufferChanged(const EditBuffer *eb)
    {
      setValue(std::any_cast<DisplayString>(getLastValue()));
    }

   protected:
    std::any getLastValue() const override
    {
      return DisplayString(
          { std::string("Select ")
                + (Application::get().getPresetManager()->getEditBuffer()->isVGISelected() ? "II" : "I"),
            0 });
    }
    OnEditBufferChangedNotifier<SelectVGButtonText> m_changed;
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
    m_map[EventSources::ParameterNameWithStateSuffix] = std::make_unique<ParameterNameWithStateSuffixEventSource>();
    m_map[EventSources::ParameterDisplayString] = std::make_unique<ParameterDisplayStringEventSource>();
    m_map[EventSources::LockStatus] = std::make_unique<CurrentParameterGroupLockStatus>();
    m_map[EventSources::MacroControlSymbol] = std::make_unique<CurrentMacroControlSymbol>();
    m_map[EventSources::MacroControlAsignment] = std::make_unique<CurrentMacroControlAsignment>();
    m_map[EventSources::MacroControlAmount] = std::make_unique<CurrentMacroControlAmount>();
    m_map[EventSources::MacroControlPosition] = std::make_unique<CurrentMacroControlPosition>();
    m_map[EventSources::MacroControlPositionText] = std::make_unique<CurrentMacroControlPositionText>();
    m_map[EventSources::MCModRange] = std::make_unique<MCModRangeEventSource>();
    m_map[EventSources::SoundHeaderText] = std::make_unique<SoundHeaderText>();
    m_map[EventSources::EditBufferName] = std::make_unique<EditBufferName>();
    m_map[EventSources::CurrentVoiceGroupName] = std::make_unique<CurrentVoiceGroupName>();
    m_map[EventSources::ParameterControlPosition] = std::make_unique<CurrentParameterControlPosition>();
    m_map[EventSources::BooleanTrue] = std::make_unique<BooleanTrue>();
    m_map[EventSources::BooleanFalse] = std::make_unique<BooleanFalse>();

    m_map[EventSources::FullSoundName] = std::make_unique<FullSoundName>();

    m_map[EventSources::isCurrentVGI] = std::make_unique<IsCurrentVGI>();
    m_map[EventSources::isCurrentVGII] = std::make_unique<IsCurrentVGII>();
    m_map[EventSources::SelectVGButtonText] = std::make_unique<SelectVGButtonText>();
    m_map[EventSources::SoundEditHeading] = std::make_unique<SoundEditHeading>();

    m_map[EventSources::MCSelectionChanged] = std::make_unique<MCSelectionChanged>();
    m_map[EventSources::MCPositionChanged] = std::make_unique<MCPositionChanged>();
    m_map[EventSources::MCAmountChanged] = std::make_unique<MCAmountChanged>();

    m_map[EventSources::ParameterValueChanged] = std::make_unique<ParameterValueChanged>();

    m_map[EventSources::IsOnlyParameterOnButton] = std::make_unique<IsOnlyParameterOnButton>();
    m_map[EventSources::IsNotOnlyParameterOnButton] = std::make_unique<IsNotOnlyParameterOnButton>();

    m_map[EventSources::DirectLoadStatus] = std::make_unique<DirectLoadStatus>();
  }

  sigc::connection EventSourceBroker::connect(EventSources source, std::function<void(std::any)> cb)
  {
    if(source == EventSources::None)
      return {};

    return m_map.at(source)->connect(cb);
  }

  std::any EventSourceBroker::evaluate(EventSources source)
  {
    return m_map[source]->getLastValue();
  }
}
