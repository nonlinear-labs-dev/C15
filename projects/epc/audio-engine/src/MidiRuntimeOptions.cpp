#include "MidiRuntimeOptions.h"

void MidiRuntimeOptions::update(const nltools::msg::Setting::MidiSettingsMessage& msg)
{
  m_receiveChannel = msg.receiveChannel;
  m_receiveSplitChannel = msg.receiveSplitChannel;

  m_sendChannel = msg.sendChannel;
  m_sendSplitChannel = msg.sendSplitChannel;

  m_receiveProgramChanges = msg.receiveProgramChange;
  m_receiveControllers = msg.receiveControllers;
  m_receiveNotes = msg.receiveNotes;

  m_sendProgramChanges = msg.sendProgramChange;
  m_sendControllers = msg.sendControllers;
  m_sendNotes = msg.sendNotes;

  m_localNotes = msg.localNotes;
  m_localControllers = msg.localControllers;
}

int MidiRuntimeOptions::getReceiveChannel() const
{
  return m_receiveChannel;
}

int MidiRuntimeOptions::getSendChannel() const
{
  return m_sendChannel;
}

bool MidiRuntimeOptions::shouldReceiveProgramChanges() const
{
  return m_receiveProgramChanges;
}

bool MidiRuntimeOptions::shouldReceiveNotes() const
{
  return m_receiveNotes;
}

bool MidiRuntimeOptions::shouldReceiveControllers() const
{
  return m_receiveControllers;
}

bool MidiRuntimeOptions::shouldSendProgramChanges() const
{
  return m_sendProgramChanges;
}

bool MidiRuntimeOptions::shouldSendNotes() const
{
  return m_sendNotes;
}

bool MidiRuntimeOptions::shouldSendControllers() const
{
  return m_sendControllers;
}

int MidiRuntimeOptions::getReceiveSplitChannel() const
{
  return m_receiveSplitChannel;
}

int MidiRuntimeOptions::getSendSplitChannel() const
{
  return m_sendSplitChannel;
}

bool MidiRuntimeOptions::shouldReceiveLocalNotes() const
{
  return m_localNotes;
}

bool MidiRuntimeOptions::shouldReceiveLocalControllers() const
{
  return m_localControllers;
}
