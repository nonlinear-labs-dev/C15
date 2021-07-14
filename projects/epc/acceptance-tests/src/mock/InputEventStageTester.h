#pragma once
#include <Application.h>
#include <synth/c15-audio-engine/dsp_host_dual.h>
#include <Types.h>

class InputEventStage;
class TCDDecoder;
class MIDIDecoder;
class DSPInterface;
class MidiRuntimeOptions;

class InputEventStageTester
{
 public:
  explicit InputEventStageTester(InputEventStage* stage);

  TCDDecoder* getTCDDecoder();
  MIDIDecoder* getMIDIDecoder();
  DSPInterface* getDSPHost();
  MidiRuntimeOptions* getOptions();
  KeyShift* getShiftableKeys();

  bool checkMIDIKeyEventEnabled(MIDIDecoder* pDecoder);
  bool checkMIDIHardwareChangeEnabled(MIDIDecoder* pDecoder);
  void onMIDIEvent();
  void onTCDEvent();

  void onTCDMessage(const MidiEvent& tcdEvent);
  void onMIDIMessage(const MidiEvent& midiEvent);

  void sendKeyDownAsMidi(TCDDecoder* pDecoder, const VoiceGroup& determinedPart);
  void convertToAndSendMIDI(TCDDecoder* pDecoder, const VoiceGroup& determinedPart);
  void sendKeyUpAsMidi(TCDDecoder* pDecoder, const VoiceGroup& determinedPart);
  void sendHardwareChangeAsMidi(TCDDecoder* pDecoder);
  void sendCCOut(int hwID, float value, int msbCC, int lsbCC);
  void doSendCCOut(uint16_t value, int msbCC, int lsbCC, int hwID);
  VoiceGroup calculateSplitPartForKeyDown(DSPInterface::InputEventSource inputEvent, int keyNumber);
  VoiceGroup calculateSplitPartForKeyUp(DSPInterface::InputEventSource inputEvent, int keyNumber);
  DSPInterface::InputEventSource getInputSourceFromParsedChannel(MidiReceiveChannel channel);

 private:
  InputEventStage* m_inputStage;
};
