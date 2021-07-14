#pragma once

#include <nltools/Types.h>
#include <nltools/messaging/Message.h>
#include "../../../audio-engine/src/Types.h"

class dsp_host_dual;

enum class Polyphony : bool
{
  Poly,
  Mono
};

enum class MockInputEventSource
{
  Singular,
  Primary,
  Secondary,
  Both
};

class DspHostDualTester
{
 public:
  explicit DspHostDualTester(dsp_host_dual* _host);

  // can be used to detect active Voices, for example with VoiceAllocation/Envelope Reset events
  unsigned int getActiveVoices(const VoiceGroup _group = VoiceGroup::Global);

  // todo: voice alloc: get assignable voices,
  unsigned int getAssignableVoices();

  // mono, unison msg generators
  void applyMonoMessage(const Polyphony _mono, const VoiceGroup _group = VoiceGroup::Global);
  void applyUnisonMessage(const unsigned int _unison, const VoiceGroup _group = VoiceGroup::Global);

  // preset generators (beware: preset messages are malformed and completely initial, just unison and mono can be used here)
  struct MalformedPresetDescriptor
  {
    const unsigned int m_unison;
    const Polyphony m_mono;
  };
  void applyMalformedSinglePreset(const MalformedPresetDescriptor& _preset);
  void applyMalformedSplitPreset(const MalformedPresetDescriptor& _partI, const MalformedPresetDescriptor& _partII);
  void applyMalformedLayerPreset(const MalformedPresetDescriptor& _preset);

  // key generators
  void applyTCDKeyDown(const unsigned int _pitch, const float _velocity, const VoiceGroup _group = VoiceGroup::Global);
  void applyTCDKeyUp(const unsigned int _pitch, const float _velocity, const VoiceGroup _group = VoiceGroup::Global);
  void applyMidiNoteOn(const unsigned int _pitch, const float _velocity, const MockInputEventSource _inputSrc,
                       const VoiceGroup _group = VoiceGroup::Global);
  void applyMidiNoteOff(const unsigned int _pitch, const float _velocity, const MockInputEventSource _inputSrc,
                        const VoiceGroup _group = VoiceGroup::Global);

  // output scanning
  struct StereoOutput
  {
    float m_left { 0.0f }, m_right { 0.0f };
    bool isSilence();
  };
  StereoOutput scanOutput(const std::vector<SampleFrame>& _audiodata);

 private:
  dsp_host_dual* m_host;
  float encodeUnisonVoice(const unsigned int _unison, const unsigned int _polyphony);
};
