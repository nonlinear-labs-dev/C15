#pragma once
#include <nltools/messaging/Message.h>

class MidiRuntimeOptions
{
 public:
  [[nodiscard]] int getReceiveChannel() const;
  [[nodiscard]] int getReceiveSplitChannel() const;
  [[nodiscard]] int getSendChannel() const;
  [[nodiscard]] int getSendSplitChannel() const;

  [[nodiscard]] bool shouldReceiveProgramChanges() const;
  [[nodiscard]] bool shouldReceiveNotes() const;
  [[nodiscard]] bool shouldReceiveControllers() const;

  [[nodiscard]] bool shouldSendProgramChanges() const;
  [[nodiscard]] bool shouldSendNotes() const;
  [[nodiscard]] bool shouldSendControllers() const;

  [[nodiscard]] bool shouldReceiveLocalNotes() const;
  [[nodiscard]] bool shouldReceiveLocalControllers() const;

  void update(const nltools::msg::Setting::MidiSettingsMessage& msg);

 private:
  int m_receiveChannel = -1;
  int m_receiveSplitChannel = -1;
  int m_sendChannel = -1;
  int m_sendSplitChannel = -1;

  bool m_receiveProgramChanges = false;
  bool m_receiveNotes = false;
  bool m_receiveControllers = false;

  bool m_sendProgramChanges = false;
  bool m_sendNotes = false;
  bool m_sendControllers = false;

  bool m_localNotes = false;
  bool m_localControllers = false;
};
