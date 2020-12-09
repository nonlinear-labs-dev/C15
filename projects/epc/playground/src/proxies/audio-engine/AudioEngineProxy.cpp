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

AudioEngineProxy::AudioEngineProxy()
{
  using namespace nltools::msg;
  onConnectionEstablished(EndPoint::AudioEngine, sigc::mem_fun(this, &AudioEngineProxy::sendEditBuffer));

  receive<HardwareSourceChangedNotification>(EndPoint::Playground, [this](auto &msg) {
    auto playController = Application::get().getPlaycontrollerProxy();
    auto id = msg.hwSource;
    auto value = msg.position;
    auto param = playController->findPhysicalControlParameterFromPlaycontrollerHWSourceID(id);
    if(auto p = dynamic_cast<PhysicalControlParameter *>(param))
    {
      playController->notifyRibbonTouch(p->getID().getNumber());
      p->onChangeFromPlaycontroller(value);
    }
  });

  const auto &pm = Application::get().getPresetManager();

  receive<Midi::ProgramChangeMessage>(EndPoint::Playground, [=](const auto &msg) {
    if(auto lock = m_programChangeRecursion.lock())
      if(auto bank = pm->getSelectedBank())
        if(msg.program < bank->getNumPresets())
          bank->selectPreset(msg.program);
  });

  pm->onBankSelection(sigc::mem_fun(this, &AudioEngineProxy::onBankSelectionChanged));
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

void AudioEngineProxy::onBankSelectionChanged(const Uuid &uuid)
{
  const auto &pm = Application::get().getPresetManager();

  m_presetSelectionConnection.disconnect();

  if(auto bank = pm->getSelectedBank())
  {
    m_presetSelectionConnection = bank->onBankChanged(sigc::mem_fun(this, &AudioEngineProxy::onBankChanged));
  }
}

void AudioEngineProxy::onBankChanged()
{
  if(auto lock = m_programChangeRecursion.lock())
  {
    const auto &pm = Application::get().getPresetManager();

    if(auto bank = pm->getSelectedBank())
    {
      if(auto preset = bank->getSelectedPreset())
      {
        uint8_t pos = bank->getPresetPosition(preset);

        if(pos < 128)
          nltools::msg::send(nltools::msg::EndPoint::AudioEngine, nltools::msg::Midi::ProgramChangeMessage { pos });
      }
    }
  }
}
