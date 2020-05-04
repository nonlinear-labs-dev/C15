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

  AudioOutput *getAudioOut() const;

  using AudioBlock = std::vector<SampleFrame>;
  using RealtimeFactor = double;
  std::tuple<AudioBlock, RealtimeFactor> measurePerformance(std::chrono::nanoseconds time);
  void resetPerformance();

  void checkFiniteness(SampleFrame *target, size_t numFrames);
  const AudioEngineOptions *getOptions() const;

 protected:
  virtual void doMidi(const MidiEvent &event) = 0;
  virtual void doAudio(SampleFrame *target, size_t numFrames) = 0;
  virtual void resetDSP();

 private:
  void process(SampleFrame *target, size_t numFrames);
  void processAudioWithoutTimestampedMidi(SampleFrame *target, size_t numFrames);
  void processAudioWithTimestampedMidi(SampleFrame *target, size_t numFrames);

  void pushMidiEvent(const MidiEvent &event);

  std::unique_ptr<MidiInput> m_in;
  std::unique_ptr<AudioOutput> m_out;
  RingBuffer<MidiEvent, 2048> m_midiRingBuffer;
  const AudioEngineOptions *m_options;
};
