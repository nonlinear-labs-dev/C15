#include <utility>

#include "GlobalEventSourceBroker.h"
#include "proxies/hwui/descriptive-layouts/LayoutFactory.h"
#include <Application.h>
#include <proxies/hwui/HWUI.h>
#include <presets/EditBuffer.h>
#include <parameters/ModulateableParameter.h>
#include <proxies/hwui/panel-unit/ButtonParameterMapping.h>
#include <presets/Bank.h>
#include <presets/Preset.h>
#include <tools/EditBufferNotifier.h>
#include <device-settings/DirectLoadSetting.h>
#include <proxies/hwui/descriptive-layouts/concrete/preset/GenericPresetList.h>
#include <proxies/hwui/descriptive-layouts/events/event-sources/hwui/HWUIEvents.h>

#include "event-sources/base/EventSource.h"
#include "event-sources/parameter/ParameterEvents.h"
#include "event-sources/parameter/MacroControlEvents.h"
#include "event-sources/parameter/ParameterUIEvents.h"
#include "event-sources/parameter/GroupEvents.h"
#include "event-sources/edit-buffer/EditBufferEvents.h"
#include "event-sources/edit-buffer/EditBufferUIEvents.h"
#include "event-sources/misc/StaticEvents.h"
#include "event-sources/misc/HWUIMappingEvents.h"
#include "event-sources/setting/SettingEvents.h"

#include "event-sources/concrete-ui/ConcretePresetListEvents.h"

namespace DescriptiveLayouts
{
  GlobalEventSourceBroker::GlobalEventSourceBroker()
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
    m_map[EventSources::EditBufferNameWithSuffix] = std::make_unique<EditBufferNameWithSuffix>();
    m_map[EventSources::ParameterControlPosition] = std::make_unique<CurrentParameterControlPosition>();

    m_map[EventSources::EditBufferMasterVolume] = std::make_unique<ParameterValue<247, VoiceGroup::Global>>();
    m_map[EventSources::EditBufferMasterTune] = std::make_unique<ParameterValue<248, VoiceGroup::Global>>();

    m_map[EventSources::IsSingleSound] = std::make_unique<IsSingleSound>();
    m_map[EventSources::IsSplitSound] = std::make_unique<IsSplitSound>();
    m_map[EventSources::IsLayerSound] = std::make_unique<IsLayerSound>();

    m_map[EventSources::VGIMasterTuneText] = std::make_unique<ParameterValue<360, VoiceGroup::I>>();
    m_map[EventSources::VGIIMasterTuneText] = std::make_unique<ParameterValue<360, VoiceGroup::II>>();
    m_map[EventSources::VGIMasterVolumeText] = std::make_unique<ParameterValue<358, VoiceGroup::I>>();
    m_map[EventSources::VGIIMasterVolumeText] = std::make_unique<ParameterValue<358, VoiceGroup::II>>();

    m_map[EventSources::BooleanTrue] = std::make_unique<BooleanTrue>();
    m_map[EventSources::BooleanFalse] = std::make_unique<BooleanFalse>();

    m_map[EventSources::isCurrentVGI] = std::make_unique<IsCurrentVGI>();
    m_map[EventSources::isCurrentVGII] = std::make_unique<IsCurrentVGII>();
    m_map[EventSources::VGIMuted] = std::make_unique<VGIMuted>();
    m_map[EventSources::VGIIMuted] = std::make_unique<VGIIMuted>();
    m_map[EventSources::VGIIIsMuted] = std::make_unique<VGIIsMuted>();
    m_map[EventSources::VGIIsMuted] = std::make_unique<VGIIIsMuted>();

    m_map[EventSources::CurrentVoiceGroupText] = std::make_unique<CurrentVoiceGroupText>();

    m_map[EventSources::SplitPointValue] = std::make_unique<SplitPointValueText>();
    m_map[EventSources::SplitPointIValue] = std::make_unique<SplitPointPartValueText<VoiceGroup::I>>();
    m_map[EventSources::SplitPointIIValue] = std::make_unique<SplitPointPartValueText<VoiceGroup::II>>();
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
    m_map[EventSources::CanLeft] = std::make_unique<PresetListEvents::PresetListHasBankLeft>();
    m_map[EventSources::CanRight] = std::make_unique<PresetListEvents::PresetListHasBankRight>();

    m_map[EventSources::DirectLoadStatus] = std::make_unique<DirectLoadStatus>();
    m_map[EventSources::SyncSplitEnabled] = std::make_unique<SplitSyncEnabled>();
    m_map[EventSources::SyncSplitDisabled] = std::make_unique<SplitSyncDisabled>();
    m_map[EventSources::isFineActive] = std::make_unique<HWUIEvents::isFineEventSource>();
    m_map[EventSources::MonoEnabledText] = std::make_unique<MonoEnabledText>();
    m_map[EventSources::MonoEnabled] = std::make_unique<MonoEnabledBool>();

    m_map[EventSources::MonoPrioText] = std::make_unique<VoicesParameterValue<365>>();
    m_map[EventSources::MonoLegatoText] = std::make_unique<VoicesParameterValue<366>>();
    m_map[EventSources::MonoGlideText] = std::make_unique<VoicesParameterValue<367>>();

    m_map[EventSources::UnisonVoicesText] = std::make_unique<UnisonVoicesText>();
    m_map[EventSources::UnisonDetuneText] = std::make_unique<VoicesParameterValue<250>>();
    m_map[EventSources::UnisonPhaseText] = std::make_unique<VoicesParameterValue<252>>();
    m_map[EventSources::UnisonPanText] = std::make_unique<VoicesParameterValue<253>>();

    m_map[EventSources::UnisonEnabled] = std::make_unique<UnisonEnabledBool>();

    m_map[EventSources::CurrentVoiceGroupLabel] = std::make_unique<CurrentVoiceGroupLabel>();

    m_map[EventSources::SoundParamsButtonText] = std::make_unique<SoundParamsButtonText>();
    m_map[EventSources::SoundVoicesButtonText] = std::make_unique<SoundVoicesButtonText>();
    m_map[EventSources::SoundMasterButtonText] = std::make_unique<SoundMasterButtonText>();

    m_map[EventSources::MonoButtonText] = std::make_unique<MonoButtonText>();
    m_map[EventSources::UnisonButtonText] = std::make_unique<UnisonButtonText>();

    m_map[EventSources::AnyLayerCrossFB] = std::make_unique<AnyLayerCrossFB>();
    m_map[EventSources::AnyLayerCrossFBToI] = std::make_unique<AnyLayerCrossFBFromII>();
    m_map[EventSources::AnyLayerCrossFBToII] = std::make_unique<AnyLayerCrossFBFromI>();

    m_map[EventSources::ToFXIUnder100] = std::make_unique<ToFXIUnder100>();
    m_map[EventSources::ToFXIOver0] = std::make_unique<ToFXIOver0>();
    m_map[EventSources::ToFXIIUnder100] = std::make_unique<ToFXIIUnder100>();
    m_map[EventSources::ToFXIIOver0] = std::make_unique<ToFXIIOver0>();

    m_map[EventSources::LayerFBState] = std::make_unique<LayerFBState>();

    m_map[EventSources::LayerFXState] = std::make_unique<LayerFXState>();
    m_map[EventSources::LayerFXOffset] = std::make_unique<LayerFXOffset>();

    m_map[EventSources::SplitPointDefaultBehaviourWithoutSync] = std::make_unique<SplitPointBehaviourIsDefaultWithoutSync>();
  }

  GlobalEventSourceBroker::~GlobalEventSourceBroker() = default;

  sigc::connection GlobalEventSourceBroker::connect(EventSources source,
                                                    const std::function<void(std::experimental::any)>& cb)
  {
    if(source == EventSources::None)
      return {};

    return m_map.at(source)->connect(cb);
  }
}
