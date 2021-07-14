#include "DspHostDualTester.h"
#include <synth/c15-audio-engine/dsp_host_dual.h>

DspHostDualTester::DspHostDualTester(dsp_host_dual* _host)
    : m_host { _host }
{
}

unsigned int DspHostDualTester::getActiveVoices(const VoiceGroup _group)
{
  // retrieving the polyphonic Gate Envelope signal from both Poly Sections/Parts (12 Voices per Part)
  // (per Voice: Gate Signal is 1.0 (immediately) after KeyPress, and (almost immediately) 0.0 after KeyRelease)

  const uint32_t gateSignalIndex = static_cast<uint32_t>(C15::Signals::Truepoly_Signals::Env_G_Sig);
  float gateSignal;

  // obtain specific Part if desired, otherwise both Parts
  switch(_group)
  {
    case VoiceGroup::I:
      gateSignal = sumUp(m_host->m_poly[0].m_signals.get_poly(gateSignalIndex));
      break;
    case VoiceGroup::II:
      gateSignal = sumUp(m_host->m_poly[1].m_signals.get_poly(gateSignalIndex));
      break;
    default:
      gateSignal = sumUp(m_host->m_poly[0].m_signals.get_poly(gateSignalIndex))
          + sumUp(m_host->m_poly[1].m_signals.get_poly(gateSignalIndex));
      break;
  }

  // having summed up all voices, we obtain the number of "active" Voices (0 ... 24)
  // the obtained value should be sample-precise and is not dependant from Sustain or Level Parameters

  return static_cast<uint8_t>(gateSignal);  // 0 : no voice active / 1, 2, ... 23, 24 : n voices active
}

unsigned int DspHostDualTester::getAssignableVoices()
{
  switch(m_host->getType())
  {
    case SoundType::Single:
      return C15::Config::total_polyphony;
    case SoundType::Split:
    case SoundType::Layer:
      return C15::Config::local_polyphony;
  }
  // should never be reached
  nltools_assertNotReached();
  return 1;
}

void DspHostDualTester::applyMonoMessage(const Polyphony _mono, const VoiceGroup _group)
{
  // prepare default message
  nltools::msg::UnmodulateableParameterChangedMessage msg { C15::PID::Mono_Grp_Enable, static_cast<float>(_mono),
                                                            VoiceGroup::Global };
  // overwrite voiceGroup in non-single sounds
  switch(m_host->getType())
  {
    case SoundType::Split:
    case SoundType::Layer:
      msg.voiceGroup = _group;
      break;
  }
  // propagate message
  m_host->localMonoEnableChg(msg);
}

void DspHostDualTester::applyUnisonMessage(const unsigned int _unison, const VoiceGroup _group)
{
  // prepare default message
  nltools::msg::UnmodulateableParameterChangedMessage msg { C15::PID::Unison_Voices, 0.0f, VoiceGroup::Global };
  // act according to sound type
  switch(m_host->getType())
  {
    case SoundType::Single:
      msg.controlPosition = encodeUnisonVoice(_unison, C15::Config::total_polyphony);
      break;
    case SoundType::Split:
    case SoundType::Layer:
      msg.controlPosition = encodeUnisonVoice(_unison, C15::Config::local_polyphony);
      msg.voiceGroup = _group;
  }
  // propagate message
  m_host->localUnisonVoicesChg(msg);
}

void DspHostDualTester::applyMalformedSinglePreset(const MalformedPresetDescriptor& _preset)
{
  // prepare message
  nltools::msg::SinglePresetMessage msg;
  msg.mono.monoEnable.controlPosition = static_cast<float>(_preset.m_mono);
  msg.unison.unisonVoices.controlPosition = encodeUnisonVoice(_preset.m_unison, C15::Config::total_polyphony);
  // propagate message
  m_host->onPresetMessage(msg);
}

void DspHostDualTester::applyMalformedSplitPreset(const MalformedPresetDescriptor& _partI,
                                                  const MalformedPresetDescriptor& _partII)
{
  // prepare message
  nltools::msg::SplitPresetMessage msg;
  msg.mono[0].monoEnable.controlPosition = static_cast<float>(_partI.m_mono);
  msg.mono[1].monoEnable.controlPosition = static_cast<float>(_partII.m_mono);
  msg.unison[0].unisonVoices.controlPosition = encodeUnisonVoice(_partI.m_unison, C15::Config::local_polyphony);
  msg.unison[1].unisonVoices.controlPosition = encodeUnisonVoice(_partII.m_unison, C15::Config::local_polyphony);
  // propagate message
  m_host->onPresetMessage(msg);
}

void DspHostDualTester::applyMalformedLayerPreset(const MalformedPresetDescriptor& _preset)
{
  // prepare message
  nltools::msg::LayerPresetMessage msg;
  msg.mono.monoEnable.controlPosition = static_cast<float>(_preset.m_mono);
  msg.unison.unisonVoices.controlPosition = encodeUnisonVoice(_preset.m_unison, C15::Config::local_polyphony);
  // propagate message
  m_host->onPresetMessage(msg);
}

void DspHostDualTester::applyTCDKeyDown(const unsigned int _pitch, const float _velocity, const VoiceGroup _group)
{
  switch(m_host->getType())
  {
    case SoundType::Single:
    case SoundType::Layer:
      m_host->onKeyDown(_pitch, _velocity, DSPInterface::InputEventSource::Internal);
      break;
    case SoundType::Split:
      m_host->onKeyDownSplit(_pitch, _velocity, _group, DSPInterface::InputEventSource::Internal);
      break;
  }
}

void DspHostDualTester::applyTCDKeyUp(const unsigned int _pitch, const float _velocity, const VoiceGroup _group)
{
  switch(m_host->getType())
  {
    case SoundType::Single:
    case SoundType::Layer:
      m_host->onKeyUp(_pitch, _velocity, DSPInterface::InputEventSource::Internal);
      break;
    case SoundType::Split:
      m_host->onKeyUpSplit(_pitch, _velocity, _group, DSPInterface::InputEventSource::Internal);
      break;
  }
}

void DspHostDualTester::applyMidiNoteOn(const unsigned int _pitch, const float _velocity,
                                        const MockInputEventSource _inputSrc, const VoiceGroup _group)
{
  // translate MockInputEventSource to External
  DSPInterface::InputEventSource inputSource;
  switch(_inputSrc)
  {
    case MockInputEventSource::Singular:
      inputSource = DSPInterface::InputEventSource::External_Use_Split;
      break;
    case MockInputEventSource::Primary:
      inputSource = DSPInterface::InputEventSource::External_Primary;
      break;
    case MockInputEventSource::Secondary:
      inputSource = DSPInterface::InputEventSource::External_Secondary;
      break;
    case MockInputEventSource::Both:
      inputSource = DSPInterface::InputEventSource::External_Both;
      break;
  }
  // act according to SoundType
  switch(m_host->getType())
  {
    case SoundType::Single:
    case SoundType::Layer:
      m_host->onKeyDown(_pitch, _velocity, inputSource);
      break;
    case SoundType::Split:
      m_host->onKeyDownSplit(_pitch, _velocity, _group, inputSource);
      break;
  }
}

void DspHostDualTester::applyMidiNoteOff(const unsigned int _pitch, const float _velocity,
                                         const MockInputEventSource _inputSrc, const VoiceGroup _group)
{
  // translate MockInputEventSource to External
  DSPInterface::InputEventSource inputSource;
  switch(_inputSrc)
  {
    case MockInputEventSource::Singular:
      inputSource = DSPInterface::InputEventSource::External_Use_Split;
      break;
    case MockInputEventSource::Primary:
      inputSource = DSPInterface::InputEventSource::External_Primary;
      break;
    case MockInputEventSource::Secondary:
      inputSource = DSPInterface::InputEventSource::External_Secondary;
      break;
    case MockInputEventSource::Both:
      inputSource = DSPInterface::InputEventSource::External_Both;
      break;
  }
  // act according to SoundType
  switch(m_host->getType())
  {
    case SoundType::Single:
    case SoundType::Layer:
      m_host->onKeyUp(_pitch, _velocity, inputSource);
      break;
    case SoundType::Split:
      m_host->onKeyUpSplit(_pitch, _velocity, _group, inputSource);
      break;
  }
}

bool DspHostDualTester::StereoOutput::isSilence()
{
  return (m_left + m_right) == 0.0f;  // is this really legit?
}

DspHostDualTester::StereoOutput DspHostDualTester::scanOutput(const std::vector<SampleFrame>& _audiodata)
{
  StereoOutput result;
  for(SampleFrame sample : _audiodata)
  {
    result.m_left += std::abs(sample.left);
    result.m_left += std::abs(sample.right);
  }
  return result;
}

float DspHostDualTester::encodeUnisonVoice(const unsigned int _unison, const unsigned int _polyphony)
{
  nltools_assertAlways(_unison > 0 && _unison <= _polyphony);
  return static_cast<float>(_unison - 1) / static_cast<float>(_polyphony - 1);
}
