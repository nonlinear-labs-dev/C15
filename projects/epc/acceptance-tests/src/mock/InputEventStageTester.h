#pragma once
#include <Application.h>
#include <synth/c15-audio-engine/dsp_host_dual.h>

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
  void onMIDIEvent(MIDIDecoder* decoder);

  void onTCDEvent(TCDDecoder* decoder);
  void sendKeyDownAsMidi(TCDDecoder* pDecoder, const VoiceGroup& determinedPart);
  void convertToAndSendMIDI(TCDDecoder* pDecoder, const VoiceGroup& determinedPart);
  void sendKeyUpAsMidi(TCDDecoder* pDecoder, const VoiceGroup& determinedPart);
  void sendHardwareChangeAsMidi(TCDDecoder* pDecoder);
  void sendCCOut(int hwID, float value, int msbCC, int lsbCC);
  void doSendCCOut(uint16_t value, int msbCC, int lsbCC);
  VoiceGroup calculateSplitPartForEvent(MIDIDecoder* pDecoder, DSPInterface::InputSource source);
  VoiceGroup calculatePartForEvent(TCDDecoder* pDecoder);
  DSPInterface::InputSource getInterfaceFromDecoder(MIDIDecoder* pDecoder);

 private:
  InputEventStage* m_inputStage;
};
