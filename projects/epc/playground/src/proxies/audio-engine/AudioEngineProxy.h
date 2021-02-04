#pragma once

#include <nltools/messaging/Message.h>
#include <parameters/messaging/ParameterMessageFactory.h>
#include <groups/MonoGroup.h>
#include <tools/RecursionGuard.h>
#include <presets/PresetManager.h>
#include "playground.h"

namespace UNDO
{
  class Transaction;
}

class EditBuffer;
class Uuid;

class AudioEngineProxy : public sigc::trackable
{
 public:
  AudioEngineProxy();

  template <typename tParameter> auto createAndSendParameterMessage(const tParameter* parameter)
  {
    if(!m_suppressParamChanges)
      sendParameterMessage(createMessage<tParameter>(parameter));
  }

  template <typename tParameter> auto createMessage(const tParameter* parameter)
  {
    return ParameterMessageFactory::createMessage<tParameter>(parameter);
  }

  template <class tMessage> void sendParameterMessage(const tMessage& msg)
  {
    if(!m_suppressParamChanges)
      send(nltools::msg::EndPoint::AudioEngine, msg);
  }

  template <typename tSettingMessage> void sendSettingMessage(const tSettingMessage& msg)
  {
    send(nltools::msg::EndPoint::AudioEngine, msg);
  }

  void sendEditBuffer();

  void freezeParameterMessages();
  void thawParameterMessages(bool send);

  static nltools::msg::LayerPresetMessage createLayerEditBufferMessage(const EditBuffer& eb);
  static nltools::msg::SplitPresetMessage createSplitEditBufferMessage(const EditBuffer& eb);
  static nltools::msg::SinglePresetMessage createSingleEditBufferMessage(const EditBuffer& eb);

 private:
  static void fillMonoPart(nltools::msg::ParameterGroups::MonoGroup& monoGroup, ParameterGroup* const& g);
  static void fillUnisonPart(nltools::msg::ParameterGroups::UnisonGroup& unisonGroup, ParameterGroup* const& g);

  void onMidiBankSelectionChanged(Uuid newMidiBankUuid);
  void sendSelectedMidiPresetAsProgramChange();
  void onPresetManagerLoaded();

  uint8_t m_lastSendProgramNumber = std::numeric_limits<uint8_t>::max();
  uint m_suppressParamChanges = 0;

  sigc::connection m_midiBankChangedConnection;
  sigc::connection m_midiBankConnection;
  RecursionGuard m_programChangeRecursion;
};
