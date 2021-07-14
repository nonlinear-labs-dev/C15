#include "synth/Synth.h"
#include "io/audio/AlsaAudioOutput.h"
#include "io/audio/AudioOutputMock.h"
#include "io/midi/AlsaMidiInput.h"
#include "io/midi/MidiInputMock.h"
#include <nltools/logging/Log.h>
#include "AudioEngineOptions.h"
#include <iostream>
#include <chrono>
#include <cmath>

Synth::Synth(const AudioEngineOptions *options)
    : m_options(options)
    , m_midiRingBuffer(2048)
    , m_tcdRingBuffer(2048)
{
}

Synth::~Synth() = default;

std::tuple<Synth::AudioBlock, Synth::RealtimeFactor> Synth::measurePerformance(std::chrono::nanoseconds time)
{
  auto start = std::chrono::high_resolution_clock::now();
  auto todo = static_cast<size_t>(time.count() * getOptions()->getSampleRate() / std::nano::den);
  std::vector<SampleFrame> samples(todo);
  doAudio(samples.data(), todo);

  auto timeUsed = std::chrono::high_resolution_clock::now() - start;
  return { std::move(samples), 1.0 * time / timeUsed };
}

void Synth::resetDSP()
{
}

template <typename T> void pushEvent(T &buffer, const MidiEvent &event, std::chrono::nanoseconds addDelay)
{
  auto &c = buffer.push(event);
  c.timestamp = std::chrono::high_resolution_clock::now() + addDelay;
}

void Synth::pushMidiEvent(const MidiEvent &event)
{
  pushEvent(m_midiRingBuffer, event, getOptions()->getAdditionalMidiDelay());
}

void Synth::pushTcdEvent(const MidiEvent &event)
{
  pushEvent(m_tcdRingBuffer, event, getOptions()->getAdditionalMidiDelay());
}

void Synth::process(SampleFrame *target, size_t numFrames)
{
  if(getOptions()->getAdditionalMidiDelay() == std::chrono::nanoseconds::zero())
    processAudioWithoutTimestampedMidi(target, numFrames);
  else
    processAudioWithTimestampedMidi(target, numFrames);

  checkFiniteness(target, numFrames);
}

void Synth::checkFiniteness(SampleFrame *target, size_t numFrames)
{
  if(numFrames)
  {
    if(!std::isfinite(target[0].left) || !std::isfinite(target[0].right))
    {
      nltools::Log::error("Infinite value detected in processed frames. Resetting.");
      resetDSP();
    }
  }
}

const AudioEngineOptions *Synth::getOptions() const
{
  return m_options;
}

void Synth::processAudioWithoutTimestampedMidi(SampleFrame *target, size_t numFrames)
{
  while(auto e = m_midiRingBuffer.peek())
  {
    doMidi(*e);
    m_midiRingBuffer.pop();
  }

  while(auto e = m_tcdRingBuffer.peek())
  {
    doTcd(*e);
    m_tcdRingBuffer.pop();
  }

  doAudio(target, numFrames);
}

template <typename Buffer, typename AudioCB, typename EventCB>
void Synth::processAudioWithTimestampedEvents(SampleFrame *target, size_t numFrames, Buffer &buffer,
                                              const AudioCB &onAudio, const EventCB &onEvent)
{
  constexpr auto nanoRec = 1.0 / std::nano::den;
  const auto sr = getOptions()->getSampleRate();
  const auto nanosToSamples = sr * nanoRec;

  auto now = std::chrono::high_resolution_clock::now();

  uint32_t done = 0;

  while(done < numFrames)
  {
    if(auto e = buffer.peek())
    {
      auto eventPos = e->timestamp;
      auto diffInNanos = eventPos - now;
      auto diffInSamples = static_cast<int32_t>(diffInNanos.count() * nanosToSamples);

      if(diffInSamples <= static_cast<int32_t>(done))
      {
        onEvent(*e);
        buffer.pop();
      }
      else
      {
        auto todoNow = std::min<size_t>(numFrames, static_cast<size_t>(diffInSamples)) - done;
        onAudio(target + done, todoNow);
        done += todoNow;
      }
    }
    else
    {
      auto todoNow = numFrames - done;
      onAudio(target + done, todoNow);
      done += todoNow;
    }
  }
}

void Synth::processAudioWithTimestampedMidi(SampleFrame *target, size_t numFrames)
{
  processAudioWithTimestampedEvents(
      target, numFrames, m_midiRingBuffer,
      [this](auto buffer, auto size) { processAudioWithTimestampedTcd(buffer, size); },
      [this](const auto &event) { doMidi(event); });
}

void Synth::processAudioWithTimestampedTcd(SampleFrame *target, size_t numFrames)
{
  processAudioWithTimestampedEvents(
      target, numFrames, m_tcdRingBuffer, [this](auto buffer, auto size) { doAudio(buffer, size); },
      [this](const auto &event) { doTcd(event); });
}
