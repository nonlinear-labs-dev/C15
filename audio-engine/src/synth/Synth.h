#pragma once

#include "Types.h"
#include "io/RingBuffer.h"

#include <memory>
#include <chrono>

class MidiInput;
class AudioOutput;

class Synth
{
 public:
  Synth();
  virtual ~Synth();

  void start();
  void stop();

  const AudioOutput *getAudioOut() const;

  virtual double measurePerformance(std::chrono::seconds time);
  void resetPerformance();

 protected:
  virtual void doMidi(const MidiEvent &event) = 0;
  virtual void doAudio(SampleFrame *target, size_t numFrames) = 0;

 private:
  void process(SampleFrame *target, size_t numFrames);
  void processAudioWithoutTimestampedMidi(SampleFrame *target, size_t numFrames);
  void processAudioWithTimestampedMidi(SampleFrame *target, size_t numFrames);

  void pushMidiEvent(const MidiEvent &event);

  std::unique_ptr<MidiInput> m_in;
  std::unique_ptr<AudioOutput> m_out;
  RingBuffer<MidiEvent, 2048> m_midiRingBuffer;
};
