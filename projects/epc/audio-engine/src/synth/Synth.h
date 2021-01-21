#pragma once

#include "Types.h"
#include "io/network/EncodedStream.h"

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
  const EncodedStream::AudioRing &getAudioRing() const;

 protected:
  virtual void doMidi(const MidiEvent &event) = 0;
  virtual void doTcd(const MidiEvent &event) = 0;
  virtual void doAudio(SampleFrame *target, size_t numFrames) = 0;
  virtual void resetDSP();

  void pushMidiEvent(const MidiEvent &event);
  void pushTcdEvent(const MidiEvent &event);

 private:
  void process(SampleFrame *target, size_t numFrames);
  void processAudioWithoutTimestampedMidi(SampleFrame *target, size_t numFrames);

  void processAudioWithTimestampedTcd(SampleFrame *target, size_t numFrames);
  void processAudioWithTimestampedMidi(SampleFrame *target, size_t numFrames);

  template <typename Buffer, typename AudioCB, typename EventCB>
  void processAudioWithTimestampedEvents(SampleFrame *target, size_t numFrames, Buffer &buffer, const AudioCB &onAudio,
                                         const EventCB &onEvent);

  std::unique_ptr<MidiInput> m_midiIn;
  std::unique_ptr<MidiInput> m_tcdIn;
  std::unique_ptr<AudioOutput> m_out;
  RingBuffer<MidiEvent, 2048> m_midiRingBuffer;
  RingBuffer<MidiEvent, 2048> m_tcdRingBuffer;
  EncodedStream::AudioRing m_audioRingBuffer;
  const AudioEngineOptions *m_options;
};
