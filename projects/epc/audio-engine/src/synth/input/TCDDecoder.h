#pragma once
#include <Types.h>
#include <synth/c15-audio-engine/dsp_host_dual.h>

class DSPInterface;
class MidiRuntimeOptions;

class TCDDecoder
{
 public:
  //use references
  TCDDecoder(DSPInterface* dsp, MidiRuntimeOptions* options, KeyShift* shift);
  bool decode(const MidiEvent& event);

  [[nodiscard]] DecoderEventType getEventType() const;
  [[nodiscard]] int getKeyOrController() const;
  [[nodiscard]] float getValue() const;

  void reset();

 protected:
  int keyOrController = -1;
  float value = 0;
  DecoderEventType m_type = DecoderEventType::UNKNOWN;

 private:
  DSPInterface* m_dsp;
  KeyShift* m_keyShift;
  MidiRuntimeOptions* m_options;

  //Constants
  constexpr static const float c_norm_vel = 1.0f / 16383.0f;  // this only applies for velocities from the LPC
  constexpr static const float c_norm_hw = 1.0f / 16000.0f;   // this only applies for hw sources from the LPC
};
