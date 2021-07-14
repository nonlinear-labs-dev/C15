#pragma once

#include "Types.h"
#include "io/RingBuffer.h"

#include <memory>
#include <vector>
#include <chrono>
#include <tuple>

class MidiInput;
class AudioOutput;
class AudioEngineOptions;

class Synth
{
 public:
  Synth(const AudioEngineOptions *options);
  virtual ~Synth();

  void start();
  void stop();

  using AudioBlock = std::vector<SampleFrame>;
  using RealtimeFactor = double;
  std::tuple<AudioBlock, RealtimeFactor> measurePerformance(std::chrono::nanoseconds time);

  void checkFiniteness(SampleFrame *target, size_t numFrames);
  const AudioEngineOptions *getOptions() const;

  void process(SampleFrame *target, size_t numFrames);
  void pushMidiEvent(const MidiEvent &event);
  void pushTcdEvent(const MidiEvent &event);

 protected:
  virtual void doTcd(const MidiEvent &event) = 0;
  virtual void doMidi(const MidiEvent &event) = 0;
  virtual void doAudio(SampleFrame *target, size_t numFrames) = 0;
  virtual void resetDSP();

 private:
  void processAudioWithoutTimestampedMidi(SampleFrame *target, size_t numFrames);

  void processAudioWithTimestampedTcd(SampleFrame *target, size_t numFrames);
  void processAudioWithTimestampedMidi(SampleFrame *target, size_t numFrames);

  template <typename Buffer, typename AudioCB, typename EventCB>
  void processAudioWithTimestampedEvents(SampleFrame *target, size_t numFrames, Buffer &buffer, const AudioCB &onAudio,
                                         const EventCB &onEvent);

  RingBuffer<MidiEvent> m_midiRingBuffer;
  RingBuffer<MidiEvent> m_tcdRingBuffer;
  const AudioEngineOptions *m_options;
};
