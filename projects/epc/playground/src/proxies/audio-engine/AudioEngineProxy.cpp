#include "AudioEngineProxy.h"
#include <device-settings/DebugLevel.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/Bank.h>
#include <presets/EditBuffer.h>
#include <nltools/messaging/Messaging.h>
#include <nltools/messaging/Message.h>
#include <parameters/MacroControlParameter.h>
#include <parameters/ModulateableParameter.h>
#include <parameters/PedalParameter.h>
#include <parameters/PitchbendParameter.h>
#include <parameters/RibbonParameter.h>
#include <parameters/ScaleParameter.h>
#include <parameters/SplitPointParameter.h>
#include <parameters/PhysicalControlParameter.h>
#include <groups/HardwareSourcesGroup.h>
#include <groups/MacroControlsGroup.h>
#include <groups/MasterGroup.h>
#include <groups/MonoGroup.h>
#include <groups/UnisonGroup.h>
#include <groups/ScaleGroup.h>
#include <device-settings/Settings.h>
#include <parameter_declarations.h>
#include <groups/SplitParameterGroups.h>
#include <proxies/playcontroller/PlaycontrollerProxy.h>
#include <device-settings/DirectLoadSetting.h>
#include <device-settings/midi/MidiChannelSettings.h>
#include <device-settings/midi/local/LocalControllersSetting.h>
#include <device-settings/midi/local/LocalNotesSetting.h>
#include <device-settings/midi/receive/MidiReceiveNotesSetting.h>
#include <device-settings/midi/receive/MidiReceiveProgramChangesSetting.h>
#include <device-settings/midi/receive/MidiReceiveAftertouchCurveSetting.h>
#include <device-settings/midi/receive/MidiReceiveVelocityCurveSetting.h>
#include <device-settings/midi/send/MidiSendProgramChangesSetting.h>
#include <device-settings/midi/send/MidiSendNotesSetting.h>
#include <device-settings/midi/send/MidiSendControllersSetting.h>
#include <device-settings/midi/receive/MidiReceiveControllersSetting.h>
#include <device-settings/midi/mappings/PedalCCMapping.h>
#include <device-settings/midi/mappings/RibbonCCMapping.h>
#include <device-settings/midi/mappings/AftertouchCCMapping.h>
#include <device-settings/midi/mappings/BenderCCMapping.h>
#include <parameters/PhysicalControlParameter.h>
#include <presets/Preset.h>
#include <device-settings/midi/mappings/EnableHighVelocityCC.h>
#include <device-settings/midi/mappings/Enable14BitSupport.h>
#include <device-settings/flac/AutoStartRecorderSetting.h>
#include <device-settings/midi/HardwareControlEnables.h>
#include <use-cases/PhysicalControlParameterUseCases.h>

AudioEngineProxy::AudioEngineProxy()
{
  using namespace nltools::msg;
  onConnectionEstablished(EndPoint::AudioEngine, sigc::mem_fun(this, &AudioEngineProxy::sendEditBuffer));
  onConnectionEstablished(EndPoint::AudioEngine,
                          sigc::mem_fun(this, &AudioEngineProxy::connectSettingsToAudioEngineMessage));

  receive<HardwareSourceChangedNotification>(EndPoint::Playground, [this](auto &msg) {
    auto proxy = Application::get().getPlaycontrollerProxy();

    if(auto param = proxy->findPhysicalControlParameterFromPlaycontrollerHWSourceID(msg.hwSource)) {
      if(auto p = dynamic_cast<PhysicalControlParameter *>(param)) {

        PhysicalControlParameterUseCases useCase(p);
        useCase.changeFromPlaycontroller(msg.position);

        proxy->notifyRibbonTouch(p->getID().getNumber());
      }
    }
  });

  const auto &pm = Application::get().getPresetManager();

  receive<Midi::ProgramChangeMessage>(EndPoint::Playground, [=](const auto &msg) {
    if(auto lock = m_programChangeRecursion.lock())
      if(auto bank = pm->findMidiSelectedBank())
      {
        setLastKnownMIDIProgramChangeNumber(static_cast<int>(msg.program));
        BankUseCases useCase(bank);
        useCase.selectPreset(msg.program);
      }
  });

  pm->onLoadHappened(sigc::mem_fun(this, &AudioEngineProxy::onPresetManagerLoaded));
}

template <typename tMsg> void fillMessageWithGlobalParams(tMsg &msg, const EditBuffer &editBuffer)
{
  size_t hwSource = 0;
  size_t globalParams = 0;
  size_t mc = 0;
  size_t mcT = 0;
  size_t modR = 0;

  auto masterParameter = dynamic_cast<const ModulateableParameter *>(
      editBuffer.findParameterByID({ C15::PID::Master_Volume, VoiceGroup::Global }));
  auto &master = msg.master.volume;
  master.id = masterParameter->getID().getNumber();
  master.controlPosition = masterParameter->getControlPositionValue();
  master.modulationAmount = masterParameter->getModulationAmount();
  master.mc = masterParameter->getModulationSource();

  auto tuneParameter = dynamic_cast<const ModulateableParameter *>(
      editBuffer.findParameterByID({ C15::PID::Master_Tune, VoiceGroup::Global }));
  auto &tune = msg.master.tune;
  tune.id = tuneParameter->getID().getNumber();
  tune.controlPosition = tuneParameter->getControlPositionValue();
  tune.modulationAmount = tuneParameter->getModulationAmount();
  tune.mc = tuneParameter->getModulationSource();

  for(auto &g : editBuffer.getParameterGroups(VoiceGroup::Global))
  {
    for(auto p : g->getParameters())
    {
      auto isScale = dynamic_cast<ScaleGroup *>(p->getParentGroup()) != nullptr;

      if(auto hwSrcParam = dynamic_cast<PhysicalControlParameter *>(p))
      {
        auto &pItem = msg.hwsources[hwSource++];
        pItem.id = p->getID().getNumber();
        pItem.controlPosition = p->getControlPositionValue();
        pItem.returnMode = hwSrcParam->getReturnMode();
      }
      else if(isScale)
      {
        auto &pItem = msg.scale[globalParams++];
        pItem.id = p->getID().getNumber();
        pItem.controlPosition = p->getControlPositionValue();
      }
      else if(auto mcParameter = dynamic_cast<MacroControlParameter *>(p))
      {
        auto &macro = msg.macros[mc++];
        macro.id = mcParameter->getID().getNumber();
        macro.controlPosition = mcParameter->getControlPositionValue();
      }
      else if(auto hwAmounts = dynamic_cast<ModulationRoutingParameter *>(p))
      {
        auto &hwAmount = msg.hwamounts[modR++];
        hwAmount.id = hwAmounts->getID().getNumber();
        hwAmount.controlPosition = hwAmounts->getControlPositionValue();
      }
      else if(MacroControlsGroup::isMacroTime(p->getID()))
      {
        auto &mcTime = msg.macrotimes[mcT++];
        mcTime.id = p->getID().getNumber();
        mcTime.controlPosition = p->getControlPositionValue();
      }
    }
  }

  nltools_assertAlways(msg.scale.size() == globalParams);
  nltools_assertAlways(msg.hwsources.size() == hwSource);
  nltools_assertAlways(msg.macros.size() == mc);
  nltools_assertAlways(msg.hwamounts.size() == modR);
  nltools_assertAlways(msg.macrotimes.size() == mcT);
}

template <typename tParameterType, typename tParameterArray>
void forEachParameterInGroup(const EditBuffer *eb, const GroupId &group, tParameterArray &array, size_t &index)
{
  for(auto &p : eb->getParameterGroupByID(group)->getParameters())
  {
    if(auto param = dynamic_cast<tParameterType *>(p))
    {
      auto &msgParam = array[index++];
      msgParam.controlPosition = param->getControlPositionValue();
      msgParam.id = param->getID().getNumber();
    }
  }
}

nltools::msg::SinglePresetMessage AudioEngineProxy::createSingleEditBufferMessage(const EditBuffer &eb)
{
  nltools::msg::SinglePresetMessage msg {};
  fillMessageWithGlobalParams(msg, eb);

  size_t mc = 0;
  size_t modR = 0;
  size_t modP = 0;
  size_t unMod = 0;

  forEachParameterInGroup<MacroControlParameter>(&eb, { "MCs", VoiceGroup::Global }, msg.macros, mc);
  forEachParameterInGroup<ModulationRoutingParameter>(&eb, { "MCM", VoiceGroup::Global }, msg.hwamounts, modR);

  for(auto &g : eb.getParameterGroups(VoiceGroup::I))
  {
    if(auto unisonGroup = dynamic_cast<UnisonGroup *>(g))
    {
      fillUnisonPart(msg.unison, unisonGroup);
    }
    else if(auto monoGroup = dynamic_cast<MonoGroup *>(g))
    {
      fillMonoPart(msg.mono, monoGroup);
    }
    else if(auto splitGroup = dynamic_cast<SplitParameterGroups *>(g))
    {
      //Ignore
    }
    else
    {
      for(auto p : g->getParameters())
      {
        if(auto modParam = dynamic_cast<ModulateableParameter *>(p))
        {
          auto &mod = msg.modulateables[modP++];
          mod.id = modParam->getID().getNumber();
          mod.controlPosition = modParam->getControlPositionValue();
          mod.modulationAmount = modParam->getModulationAmount();
          mod.mc = modParam->getModulationSource();
        }
        else
        {
          auto &unModulateable = msg.unmodulateables[unMod++];
          unModulateable.id = p->getID().getNumber();
          unModulateable.controlPosition = p->getControlPositionValue();
        }
      }
    }
  }

  nltools_assertAlways(msg.modulateables.size() == modP);
  nltools_assertAlways(msg.unmodulateables.size() == unMod);
  nltools_assertAlways(msg.macros.size() == mc);
  nltools_assertAlways(msg.hwamounts.size() == modR);
  return msg;
}

void AudioEngineProxy::fillMonoPart(nltools::msg::ParameterGroups::MonoGroup &monoGroup, ParameterGroup *const &g)
{
  auto from = g->getVoiceGroup();

  if(auto enable = g->findParameterByID({ C15::PID::Mono_Grp_Enable, from }))
  {
    auto &monoEnable = monoGroup.monoEnable;
    monoEnable.id = C15::PID::Mono_Grp_Enable;
    monoEnable.controlPosition = enable->getControlPositionValue();
  }

  if(auto prio = g->findParameterByID({ C15::PID::Mono_Grp_Prio, from }))
  {
    auto &item = monoGroup.priority;
    item.id = C15::PID::Mono_Grp_Prio;
    item.controlPosition = prio->getControlPositionValue();
  }

  if(auto legato = g->findParameterByID({ C15::PID::Mono_Grp_Legato, from }))
  {
    auto &item = monoGroup.legato;
    item.id = C15::PID::Mono_Grp_Legato;
    item.controlPosition = legato->getControlPositionValue();
  }

  if(auto glide = dynamic_cast<ModulateableParameter *>(g->findParameterByID({ C15::PID::Mono_Grp_Glide, from })))
  {
    auto &item = monoGroup.glide;
    item.id = C15::PID::Mono_Grp_Glide;
    item.controlPosition = glide->getControlPositionValue();
    item.mc = glide->getModulationSource();
    item.modulationAmount = glide->getModulationAmount();
  }
}

void AudioEngineProxy::fillUnisonPart(nltools::msg::ParameterGroups::UnisonGroup &unisonGroup, ParameterGroup *const &g)
{
  auto from = g->getVoiceGroup();

  if(auto unisonParam = g->getParameterByID({ C15::PID::Unison_Voices, from }))
  {
    auto &unisonVoices = unisonGroup.unisonVoices;
    unisonVoices.id = C15::PID::Unison_Voices;
    unisonVoices.controlPosition = unisonParam->getControlPositionValue();
  }

  if(auto unisonDetune = dynamic_cast<ModulateableParameter *>(g->getParameterByID({ C15::PID::Unison_Detune, from })))
  {
    auto &detune = unisonGroup.detune;
    detune.id = unisonDetune->getID().getNumber();
    detune.controlPosition = unisonDetune->getControlPositionValue();
    detune.mc = unisonDetune->getModulationSource();
    detune.modulationAmount = unisonDetune->getModulationAmount();
  }

  if(auto unisonPan = g->getParameterByID(
         { C15::PID::Unison_Pan, from }))  // previously was 252: Unison Phase (seems like a bugfix..?)
  {
    auto &pan = unisonGroup.pan;
    pan.id = unisonPan->getID().getNumber();
    pan.controlPosition = unisonPan->getControlPositionValue();
  }

  if(auto unisonPhase = g->getParameterByID(
         { C15::PID::Unison_Phase, from }))  // previously was 253: Unison Pan (seems like a bugfix..?)
  {
    auto &phase = unisonGroup.phase;
    phase.id = unisonPhase->getID().getNumber();
    phase.controlPosition = unisonPhase->getControlPositionValue();
  }
}

template <typename tMsg> void fillDualMessage(tMsg &msg, const EditBuffer &editBuffer)
{
  for(auto vg : { VoiceGroup::I, VoiceGroup::II })
  {
    size_t modP = 0;
    size_t unMod = 0;

    auto arrayIndex = static_cast<int>(vg);
    for(auto &g : editBuffer.getParameterGroups(vg))
    {
      if(dynamic_cast<UnisonGroup *>(g) || dynamic_cast<MonoGroup *>(g))
        continue;

      for(auto p : g->getParameters())
      {
        if(auto modParam = dynamic_cast<ModulateableParameter *>(p))
        {
          if(modParam->getID().getNumber() != C15::PID::Split_Split_Point)
          {
            auto &mod = msg.modulateables[arrayIndex][modP++];
            mod.id = modParam->getID().getNumber();
            mod.controlPosition = modParam->getControlPositionValue();
            mod.modulationAmount = modParam->getModulationAmount();
            mod.mc = modParam->getModulationSource();
          }
        }
        else
        {
          if(p->getID().getNumber() != C15::PID::Unison_Voices && p->getID().getNumber() != C15::PID::Mono_Grp_Enable)
          {
            auto &unModulateable = msg.unmodulateables[arrayIndex][unMod++];
            unModulateable.id = p->getID().getNumber();
            unModulateable.controlPosition = p->getControlPositionValue();
          }
        }
      }
    }

    nltools_assertAlways(msg.modulateables[arrayIndex].size() == modP);
    nltools_assertAlways(msg.unmodulateables[arrayIndex].size() == unMod);
  }
}

nltools::msg::SplitPresetMessage AudioEngineProxy::createSplitEditBufferMessage(const EditBuffer &eb)
{
  nltools::msg::SplitPresetMessage msg {};
  fillMessageWithGlobalParams(msg, eb);
  fillDualMessage(msg, eb);

  for(auto i = 0; i < 2; i++)
  {
    const auto vg = i == 0 ? VoiceGroup::I : VoiceGroup::II;
    auto param = dynamic_cast<ModulateableParameter *>(eb.findParameterByID({ C15::PID::Split_Split_Point, vg }));

    auto &t = msg.splitpoint[i];
    t.id = param->getID().getNumber();
    t.controlPosition = param->getControlPositionValue();
    t.modulationAmount = param->getModulationAmount();
    t.mc = param->getModulationSource();
  }

  for(auto vg : { VoiceGroup::I, VoiceGroup::II })
  {
    auto vgIndex = static_cast<int>(vg);

    if(auto monoGroup = eb.getParameterGroupByID({ "Mono", vg }))
    {
      fillMonoPart(msg.mono[vgIndex], monoGroup);
    }

    if(auto unisonGroup = eb.getParameterGroupByID({ "Unison", vg }))
    {
      fillUnisonPart(msg.unison[vgIndex], unisonGroup);
    }
  }

  return msg;
}

nltools::msg::LayerPresetMessage AudioEngineProxy::createLayerEditBufferMessage(const EditBuffer &eb)
{
  nltools::msg::LayerPresetMessage msg {};
  fillMessageWithGlobalParams(msg, eb);
  fillDualMessage(msg, eb);

  if(auto unisonGroup = eb.getParameterGroupByID({ "Unison", VoiceGroup::I }))
    fillUnisonPart(msg.unison, unisonGroup);

  if(auto monoGroup = eb.getParameterGroupByID({ "Mono", VoiceGroup::I }))
    fillMonoPart(msg.mono, monoGroup);

  return msg;
}

void AudioEngineProxy::sendEditBuffer()
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();

  switch(eb->getType())
  {
    case SoundType::Single:
      nltools::msg::send(nltools::msg::EndPoint::AudioEngine, createSingleEditBufferMessage(*eb));
      break;
    case SoundType::Split:
      nltools::msg::send(nltools::msg::EndPoint::AudioEngine, createSplitEditBufferMessage(*eb));
      break;
    case SoundType::Layer:
      nltools::msg::send(nltools::msg::EndPoint::AudioEngine, createLayerEditBufferMessage(*eb));
      break;
    case SoundType::Invalid:
      break;
  }
}

void AudioEngineProxy::freezeParameterMessages()
{
  m_suppressParamChanges++;
}

void AudioEngineProxy::thawParameterMessages(bool send)
{
  m_suppressParamChanges--;

  if(m_suppressParamChanges == 0 && send)
    sendEditBuffer();
}

void AudioEngineProxy::onMidiBankSelectionChanged(const Uuid &newMidiBankUUID)
{
  m_midiBankChangedConnection.disconnect();

  auto pm = Application::get().getPresetManager();

  if(auto midiBank = pm->findBank(newMidiBankUUID))
  {
    m_midiBankChangedConnection
        = midiBank->onBankChanged(sigc::mem_fun(this, &AudioEngineProxy::sendSelectedMidiPresetAsProgramChange));
  }
}

void AudioEngineProxy::sendSelectedMidiPresetAsProgramChange()
{
  auto pm = Application::get().getPresetManager();
  if(auto midiBank = pm->findMidiSelectedBank())
  {
    if(pm->getSelectedBank() == midiBank)
    {
      if(auto selectedPreset = midiBank->getSelectedPreset())
      {
        uint8_t presetPos = midiBank->getPresetPosition(selectedPreset);
        if(m_lastMIDIKnownProgramNumber != presetPos)
        {
          if(presetPos < 128)
          {
            m_lastMIDIKnownProgramNumber = presetPos;
            nltools::msg::Midi::ProgramChangeMessage msg {};
            msg.program = presetPos;
            msg.programType = selectedPreset->getType();
            nltools::msg::send(nltools::msg::EndPoint::AudioEngine, msg);
          }
        }
      }
    }
  }
}

void AudioEngineProxy::onPresetManagerLoaded()
{
  auto pm = Application::get().getPresetManager();
  m_midiBankConnection
      = pm->onMidiBankSelectionHappened(sigc::mem_fun(this, &AudioEngineProxy::onMidiBankSelectionChanged));
}

template <typename T> void AudioEngineProxy::subscribeToMidiSetting(Settings *s)
{
  if(auto setting = s->getSetting<T>())
  {
    m_settingConnections.push_back(
        setting->onChange(sigc::hide(sigc::mem_fun(this, &AudioEngineProxy::scheduleMidiSettingsMessage))));
  }
}

template <typename... TT> void AudioEngineProxy::subscribeToMidiSettings(Settings *s)
{
  (subscribeToMidiSetting<TT>(s), ...);
}

void AudioEngineProxy::connectSettingsToAudioEngineMessage()
{
  auto settings = Application::get().getSettings();
  m_settingConnections.clear();

  subscribeToMidiSettings<LocalNotesSetting, MidiReceiveChannelSetting, MidiReceiveChannelSplitSetting,
                          MidiReceiveProgramChangesSetting, MidiReceiveNotesSetting, MidiReceiveAftertouchCurveSetting,
                          MidiReceiveVelocityCurveSetting, MidiSendChannelSetting, MidiSendChannelSplitSetting,
                          MidiSendProgramChangesSetting, MidiSendNotesSetting, PedalCCMapping<1>, PedalCCMapping<2>,
                          PedalCCMapping<3>, PedalCCMapping<4>, RibbonCCMapping<1>, RibbonCCMapping<2>,
                          AftertouchCCMapping, BenderCCMapping, EnableHighVelocityCC, Enable14BitSupport,
                          HardwareControlEnables>(settings);

  m_settingConnections.push_back(settings->getSetting<AutoStartRecorderSetting>()->onChange([this](const Setting *s) {
    auto as = static_cast<const AutoStartRecorderSetting *>(s);
    const auto shouldAutoStart = as->get();
    auto msg = nltools::msg::Setting::FlacRecorderAutoStart {};
    msg.enabled = shouldAutoStart;
    nltools::msg::send(nltools::msg::EndPoint::AudioEngine, msg);
  }));
}

void AudioEngineProxy::scheduleMidiSettingsMessage()
{
  m_sendMidiSettingThrottler.doTask(
      [this]()
      {
        auto settings = Application::get().getSettings();
        nltools::msg::Setting::MidiSettingsMessage msg;
        msg.sendChannel = settings->getSetting<MidiSendChannelSetting>()->get();
        msg.sendSplitChannel = settings->getSetting<MidiSendChannelSplitSetting>()->get();
        msg.receiveChannel = settings->getSetting<MidiReceiveChannelSetting>()->get();
        msg.receiveSplitChannel = settings->getSetting<MidiReceiveChannelSplitSetting>()->get();

        msg.sendNotes = settings->getSetting<MidiSendNotesSetting>()->get();
        msg.sendProgramChange = settings->getSetting<MidiSendProgramChangesSetting>()->get();

        msg.receiveNotes = settings->getSetting<MidiReceiveNotesSetting>()->get();
        msg.receiveProgramChange = settings->getSetting<MidiReceiveProgramChangesSetting>()->get();

        msg.localNotes = settings->getSetting<LocalNotesSetting>()->get();

        msg.pedal1cc = settings->getSetting<PedalCCMapping<1>>()->get();
        msg.pedal2cc = settings->getSetting<PedalCCMapping<2>>()->get();
        msg.pedal3cc = settings->getSetting<PedalCCMapping<3>>()->get();
        msg.pedal4cc = settings->getSetting<PedalCCMapping<4>>()->get();
        msg.ribbon1cc = settings->getSetting<RibbonCCMapping<1>>()->get();
        msg.ribbon2cc = settings->getSetting<RibbonCCMapping<2>>()->get();
        msg.aftertouchcc = settings->getSetting<AftertouchCCMapping>()->get();
        msg.bendercc = settings->getSetting<BenderCCMapping>()->get();

        msg.highVeloCCEnabled = settings->getSetting<EnableHighVelocityCC>()->get();
        msg.highResCCEnabled = settings->getSetting<Enable14BitSupport>()->get();

        msg.hwMappings = settings->getSetting<HardwareControlEnables>()->getRaw();

        nltools::msg::send(nltools::msg::EndPoint::AudioEngine, msg);
      });
}

void AudioEngineProxy::setLastKnownMIDIProgramChangeNumber(int pc)
{
  m_lastMIDIKnownProgramNumber = pc;
}
