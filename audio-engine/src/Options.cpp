#include "Options.h"
#include <glibmm/optiongroup.h>
#include <iostream>

template <typename T>
void add(Glib::OptionGroup &mainGroup, T &ref, const std::string &longName, const char shortName,
         const std::string &desc, int flags = 0)
{
  Glib::OptionEntry o;
  o.set_long_name(longName);
  o.set_short_name(shortName);
  o.set_description(desc);
  o.set_flags(flags);
  mainGroup.add_entry(o, ref);
}

Options::Options(int &argc, char **&argv)
{


  Glib::ustring additionalMidiDelayString;

  Glib::OptionGroup mainGroup("common", "common options");
  Glib::OptionContext ctx;



  add(mainGroup, m_rate, "sample-rate", 'r', "Audio samplerate");
  add(mainGroup, m_polyphony, "polyphony", 'p', "Number of voices");
  add(mainGroup, m_midiInputDeviceName, "midi-in", 'm', "Name of the alsa midi input device");
  add(mainGroup, m_audioOutputDeviceName, "audio-out", 'a', "Name of the alsa audio output device");
  add(mainGroup, m_fatalXRuns, "fatal-xruns", 'f', "Terminate program in case of alsa underrun or overrun");
  add(mainGroup, m_measurePerformance, "measure-performance", 'e', "Measure audio-engine realtime performance ");
  add(mainGroup, additionalMidiDelayString, "additional-midi-delay", 'i',
      "incoming midi note delay (in ns)");
  add(mainGroup, m_framesPerPeriod, "frames-per-period", 's',
      "alsa audio input frames per period");
  add(mainGroup, m_numPeriods, "num-periods", 'n', "alsa audio input number of periods");
  add(mainGroup, m_alsaBufferSize, "buffer-size", 'b', "alsa audio input ring buffer size");
  add(mainGroup, m_playgroundHost, "playground-host", 'h', "Where to find the playground");

  ctx.set_main_group(mainGroup);
  ctx.set_help_enabled(true);

  if(!ctx.parse(argc, argv))
    std::cerr << ctx.get_summary();

  if(!additionalMidiDelayString.empty())
    m_additionalMidiDelay = std::chrono::nanoseconds(std::stoi(additionalMidiDelayString));
}

bool Options::areXRunsFatal() const
{
  return m_fatalXRuns;
}

std::string Options::getMidiInputDeviceName() const
{
  return m_midiInputDeviceName;
}

std::chrono::nanoseconds Options::getAdditionalMidiDelay() const
{
  return m_additionalMidiDelay;
}

std::string Options::getAudioOutputDeviceName() const
{
  return m_audioOutputDeviceName;
}

int Options::getFramesPerPeriod() const
{
  return m_framesPerPeriod;
}

int Options::getNumPeriods() const
{
  return m_numPeriods;
}

int Options::getAlsaRingBufferSize() const
{
  return m_alsaBufferSize;
}

std::string Options::getPlaygroundHost() const
{
  return m_playgroundHost;
}

int Options::getSampleRate() const
{
  return m_rate;
}

int Options::getPolyphony() const
{
  return m_polyphony;
}

bool Options::doMeasurePerformance()
{
  return m_measurePerformance;
}
