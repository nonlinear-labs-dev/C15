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

AudioEngineProxy::AudioEngineProxy()
{
  using namespace nltools::msg;
  onConnectionEstablished(EndPoint::AudioEngine, sigc::mem_fun(this, &AudioEngineProxy::sendEditBuffer));
}

void AudioEngineProxy::sendParameter(uint16_t id, tControlPositionValue value)
{
  using namespace nltools::msg;
  send(EndPoint::AudioEngine, ParameterChangedMessage(id, value));
}

void AudioEngineProxy::toggleSuppressParameterChanges(UNDO::Transaction *transaction)
{
  transaction->addSimpleCommand([=](UNDO::Command::State) mutable {
    if(std::exchange(m_suppressParamChanges, !m_suppressParamChanges))
      sendEditBuffer();
  });
}

void AudioEngineProxy::sendEditBuffer()
{
  DebugLevel::info("send preset to AudioEngine");

  nltools::msg::SetPresetMessage msg;

  size_t aftertouch = 0;
  size_t bender = 0;
  size_t macros = 0;
  size_t modulateables = 0;
  size_t pedals = 0;
  size_t ribbons = 0;
  size_t unmodulateables = 0;

  auto editBuffer = Application::get().getPresetManager()->getEditBuffer();

  for(auto &g : editBuffer->getParameterGroups())
  {
    for(auto &p : g->getParameters())
    {
      if(auto a = dynamic_cast<AftertouchParameter *>(p))
      {
        auto &t = msg.aftertouch[aftertouch++];
        t.id = a->getID();
        t.controlPosition = static_cast<float>(a->getControlPositionValue());
        t.returnMode = a->getReturnMode();
      }
      else if(auto a = dynamic_cast<PitchbendParameter *>(p))
      {
        auto &t = msg.bender[bender++];
        t.id = a->getID();
        t.controlPosition = static_cast<float>(a->getControlPositionValue());
        t.returnMode = a->getReturnMode();
      }
      else if(auto a = dynamic_cast<MacroControlParameter *>(p))
      {
        auto &t = msg.macros[macros++];
        t.id = a->getID();
        t.controlPosition = static_cast<float>(a->getControlPositionValue());
      }
      else if(auto a = dynamic_cast<ModulateableParameter *>(p))
      {
        auto &t = msg.modulateables[modulateables++];
        t.id = a->getID();
        t.controlPosition = static_cast<float>(a->getControlPositionValue());
        t.mc = a->getModulationSource();
        t.modulationAmount = static_cast<float>(a->getModulationAmount());
      }
      else if(auto a = dynamic_cast<PedalParameter *>(p))
      {
        auto &t = msg.pedals[pedals++];
        t.id = a->getID();
        t.controlPosition = static_cast<float>(a->getControlPositionValue());
        t.pedalMode = a->getPedalMode();
        t.returnMode = a->getReturnMode();
      }
      else if(auto a = dynamic_cast<RibbonParameter *>(p))
      {
        auto &t = msg.ribbons[ribbons++];
        t.id = a->getID();
        t.controlPosition = static_cast<float>(a->getControlPositionValue());
        t.ribbonReturnMode = a->getRibbonReturnMode();
        t.ribbonTouchBehaviour = a->getRibbonTouchBehaviour();
      }
      else if(auto a = dynamic_cast<Parameter *>(p))
      {
        auto &t = msg.unmodulateables[unmodulateables++];
        t.id = a->getID();
        t.controlPosition = static_cast<float>(a->getControlPositionValue());
      }
    }
  }

  nltools::msg::send(nltools::msg::EndPoint::AudioEngine, msg);
}
