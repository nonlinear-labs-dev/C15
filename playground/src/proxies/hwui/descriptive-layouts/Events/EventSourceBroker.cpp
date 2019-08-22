#include <utility>

#include "EventSourceBroker.h"
#include "proxies/hwui/descriptive-layouts/LayoutFactory.h"
#include <Application.h>
#include <proxies/hwui/HWUI.h>
#include <presets/EditBuffer.h>
#include <parameters/ModulateableParameter.h>
#include <tools/OnParameterChangedNotifier.h>
#include <proxies/hwui/panel-unit/ButtonParameterMapping.h>
#include <presets/Bank.h>
#include <presets/Preset.h>
#include <tools/EditBufferNotifier.h>
#include <device-settings/AutoLoadSelectedPreset.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Preset/GenericPresetList.h>

#include "EventSources/base/EventSource.h"
#include "EventSources/Parameter/ParameterEvents.h"
#include "EventSources/Parameter/MacroControlEvents.h"
#include "EventSources/Parameter/ParameterUIEvents.h"
#include "EventSources/Parameter/GroupEvents.h"
#include "EventSources/EditBuffer/EditBufferEvents.h"
#include "EventSources/EditBuffer/EditBufferUIEvents.h"
#include "EventSources/misc/StaticEvents.h"
#include "EventSources/misc/HWUIMappingEvents.h"
#include "EventSources/Setting/SettingEvents.h"

#include "EventSources/ConcreteUI/ConcretePresetListEvents.h"

namespace DescriptiveLayouts
{

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

    m_map[EventSources::PresetListBankName] = std::make_unique<PresetListEvents::PresetListBankName>();
    m_map[EventSources::PresetListPresetName] = std::make_unique<PresetListEvents::PresetListPresetName>();
    m_map[EventSources::PresetListHasLeftBank] = std::make_unique<PresetListEvents::PresetListHasBankLeft>();
    m_map[EventSources::PresetListHasRightBank] = std::make_unique<PresetListEvents::PresetListHasBankRight>();

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
