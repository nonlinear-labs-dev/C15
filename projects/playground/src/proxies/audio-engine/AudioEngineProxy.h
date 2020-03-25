#pragma once

#include <nltools/messaging/Message.h>
#include <parameters/messaging/ParameterMessageFactory.h>
#include <groups/MonoGroup.h>
#include "playground.h"

namespace UNDO
{
  class Transaction;
}

class AudioEngineProxy
{
 public:
  AudioEngineProxy();

  template <typename tParameter> auto createAndSendParameterMessage(const tParameter* parameter)
  {
    sendParameterMessage(createMessage<tParameter>(parameter));
  }

  template <typename tParameter> auto createMessage(const tParameter* parameter)
  {
    return ParameterMessageFactory::createMessage<tParameter>(parameter);
  }

  template <class tMessage> void sendParameterMessage(const tMessage& msg)
  {
    send(nltools::msg::EndPoint::AudioEngine, msg);
  }

  template <typename tSettingMessage> void sendSettingMessage(const tSettingMessage& msg)
  {
    send(nltools::msg::EndPoint::AudioEngine, msg);
  }

  void sendEditBuffer();

  void toggleSuppressParameterChanges(UNDO::Transaction* transaction);

  static nltools::msg::LayerPresetMessage createLayerEditBufferMessage();
  static nltools::msg::SplitPresetMessage createSplitEditBufferMessage();
  static nltools::msg::SinglePresetMessage createSingleEditBufferMessage();

 private:
  bool m_suppressParamChanges = false;

  static void fillMonoPart(nltools::msg::ParameterGroups::MonoGroup& monoGroup, ParameterGroup* const& g);
  static void fillUnisonPart(nltools::msg::ParameterGroups::UnisonGroup& unisonGroup, ParameterGroup* const& g);
};
