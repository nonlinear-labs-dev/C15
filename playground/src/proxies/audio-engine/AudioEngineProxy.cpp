#include "AudioEngineProxy.h"
#include <device-settings/DebugLevel.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <nltools/messaging/Messaging.h>
#include <nltools/messaging/Message.h>
#include <parameters/AftertouchParameter.h>
#include <parameters/MacroControlParameter.h>
#include <parameters/ModulateableParameter.h>
#include <parameters/PedalParameter.h>
#include <parameters/PitchbendParameter.h>
#include <parameters/RibbonParameter.h>
#include <parameters/ScaleParameter.h>
#include <parameters/PhysicalControlParameter.h>
#include <parameters/voice-group-master-group/VoiceGroupMasterParameter.h>
#include <groups/HardwareSourcesGroup.h>
#include <groups/MacroControlsGroup.h>
#include <groups/MasterGroup.h>
#include <groups/MonoGroup.h>
#include <groups/UnisonGroup.h>
#include <groups/ScaleGroup.h>

AudioEngineProxy::AudioEngineProxy()
{
  using namespace nltools::msg;
  onConnectionEstablished(EndPoint::AudioEngine, sigc::mem_fun(this, &AudioEngineProxy::sendEditBuffer));
}

void AudioEngineProxy::toggleSuppressParameterChanges(UNDO::Transaction *transaction)
{
  transaction->addSimpleCommand([=](UNDO::Command::State) mutable {
    if(std::exchange(m_suppressParamChanges, !m_suppressParamChanges))
      sendEditBuffer();
  });
}

constexpr auto cUnisonVoicesParameterNumber = 249;
constexpr auto cMonoEnableParameterNumber = 364;

template <typename tMsg> void fillMessageWithGlobalParams(tMsg &msg, EditBuffer *editBuffer)
{
  size_t hwSource = 0;
  size_t globalParams = 0;
  size_t mc = 0;
  size_t mcT = 0;
  size_t modR = 0;

  for(auto &g : editBuffer->getParameterGroups(VoiceGroup::Global))
  {
    for(auto p : g->getParameters())
    {
      auto isMaster = dynamic_cast<MasterGroup *>(p->getParentGroup()) != nullptr;
      auto isScale = dynamic_cast<ScaleGroup *>(p->getParentGroup()) != nullptr;

      if(auto hwSrcParam = dynamic_cast<PhysicalControlParameter *>(p))
      {
        auto &pItem = msg.hwsources[hwSource++];
        pItem.id = p->getID().getNumber();
        pItem.controlPosition = p->getControlPositionValue();
        pItem.returnMode = hwSrcParam->getReturnMode();
      }
      else if(isMaster || isScale)
      {
        auto &pItem = msg.globalparams[globalParams++];
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

  nltools_assertAlways(msg.globalparams.size() == globalParams);
  nltools_assertAlways(msg.hwsources.size() == hwSource);
  nltools_assertAlways(msg.macros.size() == mc);
  nltools_assertAlways(msg.hwamounts.size() == modR);
  nltools_assertAlways(msg.macrotimes.size() == mcT);
}

template <typename tParameterType, typename tParameterArray>
void forEachParameterInGroup(EditBuffer *eb, const GroupId &group, tParameterArray &array, size_t &index)
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

nltools::msg::SinglePresetMessage AudioEngineProxy::createSingleEditBufferMessage()
{
  nltools::msg::SinglePresetMessage msg{};
  auto editBuffer = Application::get().getPresetManager()->getEditBuffer();
  fillMessageWithGlobalParams(msg, editBuffer);

  size_t mc = 0;
  size_t modR = 0;
  size_t modP = 0;
  size_t unMod = 0;

  forEachParameterInGroup<MacroControlParameter>(editBuffer, { "MCs", VoiceGroup::Global }, msg.macros, mc);
  forEachParameterInGroup<ModulationRoutingParameter>(editBuffer, { "MCM", VoiceGroup::Global }, msg.hwamounts, modR);

  for(auto &g : editBuffer->getParameterGroups(VoiceGroup::I))
  {
    if(auto unisonGroup = dynamic_cast<UnisonGroup *>(g))
    {
      fillUnisonPart(msg.unison, unisonGroup);
    }
    else if(auto monoGroup = dynamic_cast<MonoGroup *>(g))
    {
      fillMonoPart(msg.mono, monoGroup);
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

  if(auto enable = g->findParameterByID({ cMonoEnableParameterNumber, from }))
  {
    auto &monoEnable = monoGroup.monoEnable;
    monoEnable.id = cMonoEnableParameterNumber;
    monoEnable.controlPosition = enable->getControlPositionValue();
  }

  if(auto prio = g->findParameterByID({ 365, from }))
  {
    auto &item = monoGroup.priority;
    item.id = 365;
    item.controlPosition = prio->getControlPositionValue();
  }

  if(auto legato = g->findParameterByID({ 366, from }))
  {
    auto &item = monoGroup.legato;
    item.id = 366;
    item.controlPosition = legato->getControlPositionValue();
  }

  if(auto glide = dynamic_cast<ModulateableParameter *>(g->findParameterByID({ 367, from })))
  {
    auto &item = monoGroup.glide;
    item.id = 367;
    item.controlPosition = glide->getControlPositionValue();
    item.mc = glide->getModulationSource();
    item.modulationAmount = glide->getModulationAmount();
  }
}

void AudioEngineProxy::fillUnisonPart(nltools::msg::ParameterGroups::UnisonGroup &unisonGroup, ParameterGroup *const &g)
{
  auto from = g->getVoiceGroup();

  if(auto unisonParam = g->getParameterByID({ cUnisonVoicesParameterNumber, from }))
  {
    auto &unisonVoices = unisonGroup.unisonVoices;
    unisonVoices.id = cUnisonVoicesParameterNumber;
    unisonVoices.controlPosition = unisonParam->getControlPositionValue();
  }

  if(auto unisonDetune = dynamic_cast<ModulateableParameter *>(g->getParameterByID({ 250, from })))
  {
    auto &detune = unisonGroup.detune;
    detune.id = unisonDetune->getID().getNumber();
    detune.controlPosition = unisonDetune->getControlPositionValue();
    detune.mc = unisonDetune->getModulationSource();
    detune.modulationAmount = unisonDetune->getModulationAmount();
  }

  if(auto unisonPan = g->getParameterByID({ 252, from }))
  {
    auto &pan = unisonGroup.pan;
    pan.id = unisonPan->getID().getNumber();
    pan.controlPosition = unisonPan->getControlPositionValue();
  }

  if(auto unisonPhase = g->getParameterByID({ 253, from }))
  {
    auto &phase = unisonGroup.phase;
    phase.id = unisonPhase->getID().getNumber();
    phase.controlPosition = unisonPhase->getControlPositionValue();
  }
}

template <typename tMsg> void fillDualMessage(tMsg &msg, EditBuffer *editBuffer)
{
  for(auto vg : { VoiceGroup::I, VoiceGroup::II })
  {
    size_t modP = 0;
    size_t unMod = 0;

    auto arrayIndex = static_cast<int>(vg);
    for(auto &g : editBuffer->getParameterGroups(vg))
    {
      if(dynamic_cast<UnisonGroup *>(g) || dynamic_cast<MonoGroup *>(g))
        continue;

      for(auto p : g->getParameters())
      {
        if(auto modParam = dynamic_cast<ModulateableParameter *>(p))
        {
          auto &mod = msg.modulateables[arrayIndex][modP++];
          mod.id = modParam->getID().getNumber();
          mod.controlPosition = modParam->getControlPositionValue();
          mod.modulationAmount = modParam->getModulationAmount();
          mod.mc = modParam->getModulationSource();
        }
        else
        {
          if(p->getID().getNumber() != cUnisonVoicesParameterNumber
             && p->getID().getNumber() != cMonoEnableParameterNumber)
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

nltools::msg::SplitPresetMessage AudioEngineProxy::createSplitEditBufferMessage()
{
  nltools::msg::SplitPresetMessage msg{};
  auto editBuffer = Application::get().getPresetManager()->getEditBuffer();
  fillMessageWithGlobalParams(msg, editBuffer);

  fillDualMessage(msg, editBuffer);

  if(auto sp = editBuffer->getSplitPoint())
  {
    auto &t = msg.splitpoint;
    t.id = sp->getID().getNumber();
    t.controlPosition = sp->getControlPositionValue();
    t.modulationAmount = sp->getModulationAmount();
    t.mc = sp->getModulationSource();
  }

  for(auto vg : { VoiceGroup::I, VoiceGroup::II })
  {
    auto vgIndex = static_cast<int>(vg);

    if(auto monoGroup = editBuffer->getParameterGroupByID({ "Mono", vg }))
    {
      fillMonoPart(msg.mono[vgIndex], monoGroup);
    }

    if(auto unisonGroup = editBuffer->getParameterGroupByID({ "Unison", vg }))
    {
      fillUnisonPart(msg.unison[vgIndex], unisonGroup);
    }
  }

  return msg;
}

nltools::msg::LayerPresetMessage AudioEngineProxy::createLayerEditBufferMessage()
{
  nltools::msg::LayerPresetMessage msg{};
  auto editBuffer = Application::get().getPresetManager()->getEditBuffer();
  fillMessageWithGlobalParams(msg, editBuffer);
  fillDualMessage(msg, editBuffer);

  if(auto unisonGroup = editBuffer->getParameterGroupByID({ "Unison", VoiceGroup::I }))
    fillUnisonPart(msg.unison, unisonGroup);

  if(auto monoGroup = editBuffer->getParameterGroupByID({ "Mono", VoiceGroup::I }))
    fillMonoPart(msg.mono, monoGroup);

  return msg;
}

void AudioEngineProxy::sendEditBuffer()
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  switch(eb->getType())
  {
    case SoundType::Single:
      nltools::msg::send(nltools::msg::EndPoint::AudioEngine, createSingleEditBufferMessage());
      break;
    case SoundType::Split:
      nltools::msg::send(nltools::msg::EndPoint::AudioEngine, createSplitEditBufferMessage());
      break;
    case SoundType::Layer:
      nltools::msg::send(nltools::msg::EndPoint::AudioEngine, createLayerEditBufferMessage());
      break;
  }

  Application::get().getSettings()->sendToLPC();
}

template <> nltools::msg::SinglePresetMessage AudioEngineProxy::createMessage<nltools::msg::SinglePresetMessage>()
{
  return AudioEngineProxy::createSingleEditBufferMessage();
}

template <> nltools::msg::SplitPresetMessage AudioEngineProxy::createMessage<nltools::msg::SplitPresetMessage>()
{
  return AudioEngineProxy::createSplitEditBufferMessage();
}

template <> nltools::msg::LayerPresetMessage AudioEngineProxy::createMessage<nltools::msg::LayerPresetMessage>()
{
  return AudioEngineProxy::createLayerEditBufferMessage();
}