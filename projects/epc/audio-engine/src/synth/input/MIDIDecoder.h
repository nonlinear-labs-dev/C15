#pragma once
#include <Types.h>
#include <synth/c15-audio-engine/midi_handle.h>
#include <parameter_descriptor.h>
#include <array>
#include <nltools/Types.h>

class DSPInterface;
class MidiRuntimeOptions;

class MIDIDecoder
{
 public:
  enum class MidiHWChangeSpecialCases
  {
    ChannelPitchbend,
    Aftertouch,
    PitchbendUp,
    PitchbendDown,
    CC
  };

  struct ReceivedHWMidiEvent
  {
    int receivedCC = -1;  //MSB - ControlID
    MidiHWChangeSpecialCases cases;
    std::array<uint8_t, 2> undecodedValueBytes;  //values are MSB, LSB
  };

  MIDIDecoder(DSPInterface* dsp, MidiRuntimeOptions* options);
  bool decode(const MidiEvent& event);

  [[nodiscard]] DecoderEventType getEventType() const;
  [[nodiscard]] int getKeyOrControl() const;
  [[nodiscard]] float getValue() const;
  [[nodiscard]] MidiReceiveChannel getChannel() const;
  [[nodiscard]] ReceivedHWMidiEvent getHWChangeStruct() const;

  void reset();

 private:
  using CC_Range_7_Bit = Midi::FullCCRange<Midi::Formats::_7_Bits_>;
  using CC_Range_14_Bit = Midi::clipped14BitCCRange;
  using CC_Range_Bender = Midi::FullCCRange<Midi::Formats::_14_Bits_>;
  using CC_Range_Vel = Midi::clipped14BitVelRange;

 protected:
  int keyOrControl = -1;
  uint8_t m_MidiLSB = 0;

  float value = 0;  // value or velocity
  ReceivedHWMidiEvent hwResult;
  DecoderEventType m_type = DecoderEventType::UNKNOWN;
  MidiReceiveChannel m_midiChannel = MidiReceiveChannel::None;

 private:
  DSPInterface* m_dsp;
  MidiRuntimeOptions* m_options;

  void processMidiForHWSource(int id, uint32_t _data);

  template <typename Range> void processBipolarMidiController(const uint32_t dataByte, int id);

  template <typename Range> void processUnipolarMidiController(const uint32_t dataByte, int id);
  static MidiReceiveChannel statusToChannel(const uint8_t status);
  void handleIncommingCC(const MidiEvent& event);
};