#include <utility>

#include "EventSourceBroker.h"
#include "proxies/hwui/descriptive-layouts/LayoutFactory.h"
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
#include <proxies/hwui/descriptive-layouts/Concrete/Preset/GenericPresetList.h>

#include "EventSources/base/EventSource.h"
#include "EventSources/Parameter/MacroControlEvents.h"
#include "EventSources/Parameter/ParameterEvents.h"
#include "EventSources/Parameter/GroupEvents.h"
#include "EventSources/EditBuffer/EditBufferEvents.h"

namespace DescriptiveLayouts
{
  using DisplayString = std::pair<Glib::ustring, int>;

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
















  class EditBufferName : public EventSource<DisplayString>
  {
   protected:
    std::experimental::any getLastValue() const override
    {
      auto name = Application::get().getPresetManager()->getEditBuffer()->getName();
      return DisplayString{ name, 0 };
    }
  };

  class CurrentVoiceGroupName : public EventSource<DisplayString>
  {
   protected:
    std::experimental::any getLastValue() const override
    {
      auto name = Application::get().getPresetManager()->getEditBuffer()->getCurrentVoiceGroupName();
      return DisplayString{ name, 0 };
    }
  };

  class BooleanTrue : public EventSource<bool>
  {
   protected:
    std::experimental::any getLastValue() const override
    {
      return std::experimental::any(true);
    }
  };

  class BooleanFalse : public EventSource<bool>
  {
   protected:
    std::experimental::any getLastValue() const override
    {
      return std::experimental::any(false);
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
    std::experimental::any getLastValue() const override
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
    std::experimental::any getLastValue() const override
    {
      return std::experimental::any(SiSc::EB::getCurrentParameter()->isValueChangedFromLoaded());
    }

    OnParameterChangedNotifier<ParameterValueChanged> m_paramChanged;
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
      setValue(std::experimental::any_cast<bool>(getLastValue()));
    }

   protected:
    std::experimental::any getLastValue() const override
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
      setValue(std::experimental::any_cast<bool>(getLastValue()));
    }

   protected:
    std::experimental::any getLastValue() const override
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
      setValue(std::experimental::any_cast<DisplayString>(getLastValue()));
    }

   protected:
    std::experimental::any getLastValue() const override
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
      setValue(std::experimental::any_cast<DisplayString>(getLastValue()));
    }

   protected:
    std::experimental::any getLastValue() const override
    {
      return DisplayString(
          { std::string("Select ")
                + (Application::get().getPresetManager()->getEditBuffer()->isVGISelected() ? "II" : "I"),
            0 });
    }
    OnEditBufferChangedNotifier<SelectVGButtonText> m_changed;
  };

  class PresetListBase : public sigc::trackable
  {
   public:
    PresetListBase()
    {
      m_layoutConnection = Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().onLayoutInstantiated(
          sigc::mem_fun(this, &PresetListBase::onLayoutChanged));
    }

    ~PresetListBase()
    {
      m_layoutConnection.disconnect();
      m_presetListConnection.disconnect();
    }

   protected:
    void onLayoutChanged(Layout *layout)
    {
      m_presetListConnection.disconnect();
      if(auto genericLayout = dynamic_cast<GenericLayout *>(layout))
      {
        if(auto presetlist = genericLayout->findControlOfType<GenericPresetList>())
        {
          m_presetListConnection = presetlist->onChange(sigc::mem_fun(this, &PresetListBase::onPresetListChanged));
        }
      }
    }

    virtual void onPresetListChanged(GenericPresetList *pl) = 0;

    sigc::connection m_layoutConnection;
    sigc::connection m_presetListConnection;
  };

  class PresetListHasBankLeft : public EventSource<bool>, public PresetListBase
  {
   protected:
    void onPresetListChanged(GenericPresetList *pl) override
    {
      if(auto selection = pl->getPresetAtSelected())
      {
        if(auto bank = dynamic_cast<Bank *>(selection->getParent()))
        {
          auto pm = Application::get().getPresetManager();
          auto next = pm->getBankAt(pm->getBankPosition(bank->getUuid()) - 1);
          setValue(next != nullptr);
        }
      }
    }
  };

  class PresetListHasBankRight : public EventSource<bool>, public PresetListBase
  {
   protected:
    void onPresetListChanged(GenericPresetList *pl) override
    {
      if(auto selection = pl->getPresetAtSelected())
      {
        if(auto bank = dynamic_cast<Bank *>(selection->getParent()))
        {
          auto pm = Application::get().getPresetManager();
          auto next = pm->getBankAt(pm->getBankPosition(bank->getUuid()) + 1);
          setValue(next != nullptr);
        }
      }
    }
  };

  class PresetListBankName : public EventSource<DisplayString>, public PresetListBase
  {
   public:
    PresetListBankName()
    {
      setValue({ "No Bank", 0 });
    }

   protected:
    void onPresetListChanged(GenericPresetList *pl) override
    {
      if(pl == nullptr)
        return;

      if(auto selection = pl->getPresetAtSelected())
      {
        if(auto bank = dynamic_cast<Bank *>(selection->getParent()))
        {
          setValue({ bank->getName(true), 0 });
        }
      }
    }
  };

  class PresetListPresetName : public EventSource<DisplayString>, public PresetListBase
  {
   public:
    PresetListPresetName()
    {
      setValue({ "No Preset", 0 });
    }

   protected:
    void onPresetListChanged(GenericPresetList *pl) override
    {
      if(auto selection = pl->getPresetAtSelected())
      {
        setValue({ selection->getName(), 0 });
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
    m_map[EventSources::IsBipolar] = std::make_unique<ParameterIsBipolarEventSource>();
    m_map[EventSources::ParameterName] = std::make_unique<ParameterNameEventSource>();
    m_map[EventSources::ParameterNameWithStateSuffix] = std::make_unique<ParameterNameWithStateSuffixEventSource>();
    m_map[EventSources::ParameterDisplayString] = std::make_unique<ParameterDisplayStringEventSource>();
    m_map[EventSources::LockStatus] = std::make_unique<CurrentParameterGroupLockStatus>();
    m_map[EventSources::MacroControlSymbol] = std::make_unique<CurrentMacroControlSymbol>();
    m_map[EventSources::MacroControlAssignment] = std::make_unique<CurrentMacroControlAssignment>();
    m_map[EventSources::MacroControlAmount] = std::make_unique<CurrentMacroControlAmount>();
    m_map[EventSources::MacroControlPosition] = std::make_unique<CurrentMacroControlPosition>();
    m_map[EventSources::MacroControlPositionText] = std::make_unique<CurrentMacroControlPositionText>();
    m_map[EventSources::MCModRange] = std::make_unique<CurrentModParamModRangeEventSource>();
    m_map[EventSources::EditBufferTypeText] = std::make_unique<EditBufferTypeStringEvent>();
    m_map[EventSources::EditBufferName] = std::make_unique<EditBufferName>();
    m_map[EventSources::CurrentVoiceGroupName] = std::make_unique<CurrentVoiceGroupName>();
    m_map[EventSources::ParameterControlPosition] = std::make_unique<CurrentParameterControlPosition>();
    m_map[EventSources::BooleanTrue] = std::make_unique<BooleanTrue>();
    m_map[EventSources::BooleanFalse] = std::make_unique<BooleanFalse>();

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

    m_map[EventSources::PresetListBankName] = std::make_unique<PresetListBankName>();
    m_map[EventSources::PresetListPresetName] = std::make_unique<PresetListPresetName>();

    m_map[EventSources::PresetListHasLeftBank] = std::make_unique<PresetListHasBankLeft>();
    m_map[EventSources::PresetListHasRightBank] = std::make_unique<PresetListHasBankRight>();

    m_map[EventSources::DirectLoadStatus] = std::make_unique<DirectLoadStatus>();
  }

  sigc::connection EventSourceBroker::connect(EventSources source, std::function<void(std::experimental::any)> cb)
  {
    if(source == EventSources::None)
      return {};

    return m_map.at(source)->connect(cb);
  }

  std::experimental::any EventSourceBroker::evaluate(EventSources source)
  {
    return m_map[source]->getLastValue();
  }
}
