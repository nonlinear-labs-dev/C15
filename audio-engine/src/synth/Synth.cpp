#include "synth/Synth.h"
#include "io/audio/AlsaAudioOutput.h"
#include "io/audio/AudioOutputMock.h"
#include "io/midi/AlsaMidiInput.h"
#include "io/midi/TestMidiInput.h"
#include "io/midi/MidiInputMock.h"
#include "io/Log.h"
#include "Options.h"
#include "main.h"

#include <iostream>
#include <chrono>

Synth::Synth()
{
  auto options = getOptions();
  auto inDeviceName = options->getMidiInputDeviceName();
  auto outDeviceName = options->getAudioOutputDeviceName();

  if(outDeviceName.empty())
    m_out = std::make_unique<AudioOutputMock>();
  else
    m_out = std::make_unique<AlsaAudioOutput>(outDeviceName, [=](auto buf, auto length) { process(buf, length); });

  if(auto distance = options->testNotesDistance())
    m_in = std::make_unique<TestMidiInput>(distance, [=](auto event) { pushMidiEvent(event); });
  else if(inDeviceName.empty())
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
  auto numSamples = static_cast<size_t>(getOptions()->getSampleRate());
  std::vector<SampleFrame> samples(numSamples);

  auto start = std::chrono::high_resolution_clock::now();
  for(int i = 0; i < time.count(); i++)
  {
    for(int v = 0; v < getOptions()->getPolyphony(); v++)
    {
      MidiEvent e;
      e.raw[0] = 0x90;
      e.raw[1] = v + 49;
      e.raw[2] = 100;
      doMidi(e);
    }
    doAudio(samples.data(), numSamples);
  }

  auto timeUsed = std::chrono::high_resolution_clock::now() - start;
  return 1.0 * time / timeUsed;
}

void Synth::pushMidiEvent(const MidiEvent &event)
{
  auto &c = m_midiRingBuffer.push(event);
  c.timestamp = std::chrono::high_resolution_clock::now() + getOptions()->getAdditionalMidiDelay();
}

void Synth::process(SampleFrame *target, size_t numFrames)
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
