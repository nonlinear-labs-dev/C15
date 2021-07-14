#include "InputEventStageTester.h"
#include <synth/input/InputEventStage.h>
#include <synth/input/MIDIDecoder.h>
#include <synth/input/TCDDecoder.h>

InputEventStageTester::InputEventStageTester(InputEventStage* stage)
    : m_inputStage { stage }
{
}

TCDDecoder* InputEventStageTester::getTCDDecoder()
{
  return &m_inputStage->m_tcdDecoder;
}

MIDIDecoder* InputEventStageTester::getMIDIDecoder()
{
  return &m_inputStage->m_midiDecoder;
}

DSPInterface* InputEventStageTester::getDSPHost()
{
  return m_inputStage->m_dspHost;
}

MidiRuntimeOptions* InputEventStageTester::getOptions()
{
  return m_inputStage->m_options;
}

KeyShift* InputEventStageTester::getShiftableKeys()
{
  return &m_inputStage->m_shifteable_keys;
}

bool InputEventStageTester::checkMIDIKeyEventEnabled(MIDIDecoder* pDecoder)
{
  return m_inputStage->checkMIDIKeyEventEnabled(pDecoder);
}

bool InputEventStageTester::checkMIDIHardwareChangeEnabled(MIDIDecoder* pDecoder)
{
  return m_inputStage->checkMIDIHardwareChangeChannelMatches(pDecoder);
}

void InputEventStageTester::onMIDIEvent()
{
  m_inputStage->onMIDIEvent();
}

void InputEventStageTester::onTCDEvent()
{
  m_inputStage->onTCDEvent();
}

void InputEventStageTester::onTCDMessage(const MidiEvent& tcdEvent)
{
  m_inputStage->onTCDMessage(tcdEvent);
}

void InputEventStageTester::onMIDIMessage(const MidiEvent& midiEvent)
{
  m_inputStage->onMIDIMessage(midiEvent);
}

void InputEventStageTester::sendKeyDownAsMidi(TCDDecoder* pDecoder, const VoiceGroup& determinedPart)
{
  m_inputStage->sendKeyDownAsMidi(pDecoder, determinedPart);
}

void InputEventStageTester::convertToAndSendMIDI(TCDDecoder* pDecoder, const VoiceGroup& determinedPart)
{
  m_inputStage->convertToAndSendMIDI(pDecoder, determinedPart);
}

void InputEventStageTester::sendKeyUpAsMidi(TCDDecoder* pDecoder, const VoiceGroup& determinedPart)
{
  m_inputStage->sendKeyDownAsMidi(pDecoder, determinedPart);
}

void InputEventStageTester::sendHardwareChangeAsMidi(TCDDecoder* pDecoder)
{
  m_inputStage->sendHardwareChangeAsMidi(0, 0);
}

void InputEventStageTester::sendCCOut(int hwID, float value, int msbCC, int lsbCC)
{
  m_inputStage->sendCCOut(hwID, value, msbCC, lsbCC);
}

void InputEventStageTester::doSendCCOut(uint16_t value, int msbCC, int lsbCC, int hwID)
{
  m_inputStage->doSendCCOut(value, msbCC, lsbCC, hwID);
}

VoiceGroup InputEventStageTester::calculateSplitPartForKeyDown(DSPInterface::InputEventSource inputEvent, int keyNumber)
{
  return m_inputStage->calculateSplitPartForKeyDown(inputEvent, keyNumber);
}

VoiceGroup InputEventStageTester::calculateSplitPartForKeyUp(DSPInterface::InputEventSource inputEvent, int keyNumber)
{
  return m_inputStage->calculateSplitPartForKeyUp(inputEvent, keyNumber);
}

DSPInterface::InputEventSource InputEventStageTester::getInputSourceFromParsedChannel(MidiReceiveChannel channel)
{
  return m_inputStage->getInputSourceFromParsedChannel(channel);
}
