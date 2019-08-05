#include "synth/Synth.h"
#include "io/audio/AlsaAudioOutput.h"
#include "io/audio/AudioOutputMock.h"
#include "io/midi/AlsaMidiInput.h"
#include "io/midi/MidiInputMock.h"
#include <nltools/logging/Log.h>
#include "Options.h"
#include "main.h"

#include <iostream>
#include <chrono>
#include <cmath>

Synth::Synth()
{
  auto options = getOptions();
  auto inDeviceName = options->getMidiInputDeviceName();
  auto outDeviceName = options->getAudioOutputDeviceName();

  if(outDeviceName.empty())
    m_out = std::make_unique<AudioOutputMock>();
  else
    m_out = std::make_unique<AlsaAudioOutput>(outDeviceName, [=](auto buf, auto length) { process(buf, length); });

  if(inDeviceName.empty())
    m_in = std::make_unique<MidiInputMock>([=](auto) {});
  else
    m_in = std::make_unique<AlsaMidiInput>(inDeviceName, [=](auto event) { pushMidiEvent(event); });
}

Synth::~Synth() = default;

void Synth::start()
{
  m_in->start();
  m_out->start();
}

void Synth::stop()
{
  m_in->stop();
  m_out->stop();
}

const AudioOutput *Synth::getAudioOut() const
{
  return m_out.get();
}

double Synth::measurePerformance(std::chrono::seconds time)
{
  auto start = std::chrono::high_resolution_clock::now();
  auto numSamples = static_cast<size_t>(time.count() * getOptions()->getSampleRate());
  std::vector<SampleFrame> samples(numSamples);
  doAudio(samples.data(), numSamples);
  auto timeUsed = std::chrono::high_resolution_clock::now() - start;
  return 1.0 * time / timeUsed;
}

void Synth::resetPerformance()
{
  m_out->resetPerformance();
}

void Synth::resetDSP()
{
}

void Synth::pushMidiEvent(const MidiEvent &event)
{
  auto &c = m_midiRingBuffer.push(event);
  c.timestamp = std::chrono::high_resolution_clock::now() + getOptions()->getAdditionalMidiDelay();
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

void Synth::processAudioWithoutTimestampedMidi(SampleFrame *target, size_t numFrames)
{
  while(auto e = m_midiRingBuffer.peek())
  {
    doMidi(*e);
    m_midiRingBuffer.pop();
  }

  doAudio(target, numFrames);
}

void Synth::processAudioWithTimestampedMidi(SampleFrame *target, size_t numFrames)
{
  constexpr auto nanoRec = 1.0 / std::nano::den;
  const auto sr = getOptions()->getSampleRate();
  const auto nanosToSamples = sr * nanoRec;

  auto now = std::chrono::high_resolution_clock::now();

  uint32_t done = 0;

  while(done < numFrames)
  {
    if(auto e = m_midiRingBuffer.peek())
    {
      auto eventPos = e->timestamp;
      auto diffInNanos = eventPos - now;
      auto diffInSamples = static_cast<int32_t>(diffInNanos.count() * nanosToSamples);

      if(diffInSamples <= static_cast<int32_t>(done))
      {
        doMidi(*e);
        m_midiRingBuffer.pop();
      }
      else
      {
        auto todoNow = std::min<size_t>(numFrames, static_cast<size_t>(diffInSamples)) - done;
        doAudio(target + done, todoNow);
        done += todoNow;
      }
    }
    else
    {
      auto todoNow = numFrames - done;
      doAudio(target + done, todoNow);
      done += todoNow;
    }
  }
}
