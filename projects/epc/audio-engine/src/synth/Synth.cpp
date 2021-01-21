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
{
  auto outDeviceName = options->getAudioOutputDeviceName();

  if(outDeviceName.empty())
    m_out = std::make_unique<AudioOutputMock>();
  else
    m_out = std::make_unique<AlsaAudioOutput>(options, outDeviceName,
                                              [=](auto buf, auto length) { process(buf, length); });

  auto inMidiDeviceName = options->getMidiInputDeviceName();
  if(inMidiDeviceName.empty())
    m_midiIn = std::make_unique<MidiInputMock>([=](auto) {});
  else
    m_midiIn = std::make_unique<AlsaMidiInput>(inMidiDeviceName, [=](auto event) { pushMidiEvent(event); });

  auto inTcdDeviceName = options->getTcdInputDeviceName();
  if(inTcdDeviceName.empty())
    m_tcdIn = std::make_unique<MidiInputMock>([=](auto) {});
  else
    m_tcdIn = std::make_unique<AlsaMidiInput>(inTcdDeviceName, [=](auto event) { pushTcdEvent(event); });
}

Synth::~Synth() = default;

void Synth::start()
{
  m_midiIn->start();
  m_tcdIn->start();
  m_out->start();
}

void Synth::stop()
{
  m_out->stop();
  m_tcdIn->stop();
  m_midiIn->stop();
}

AudioOutput *Synth::getAudioOut() const
{
  return m_out.get();
}

std::tuple<Synth::AudioBlock, Synth::RealtimeFactor> Synth::measurePerformance(std::chrono::nanoseconds time)
{
  auto start = std::chrono::high_resolution_clock::now();
  auto todo = static_cast<size_t>(time.count() * getOptions()->getSampleRate() / std::nano::den);
  std::vector<SampleFrame> samples(todo);
  doAudio(samples.data(), todo);

  auto timeUsed = std::chrono::high_resolution_clock::now() - start;
  return { std::move(samples), 1.0 * time / timeUsed };
}

void Synth::resetPerformance()
{
  m_out->resetPerformance();
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
  m_audioRingBuffer.push(target, numFrames);
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

const Synth::AudioRing &Synth::getAudioRing() const
{
  return m_audioRingBuffer;
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
