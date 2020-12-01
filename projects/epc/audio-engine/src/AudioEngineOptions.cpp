#include "AudioEngineOptions.h"
#include <glibmm/optiongroup.h>
#include <glibmm/optionentry.h>
#include <glibmm/optioncontext.h>
#include <iostream>
#include <nltools/logging/Log.h>
#include <nltools/ErrorCodes.h>
#include <giomm.h>

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

AudioEngineOptions::AudioEngineOptions(int &argc, char **&argv)
{

  Glib::ustring additionalMidiDelayString;

  Glib::OptionGroup mainGroup("common", "common options");
  Glib::OptionContext ctx;

  add(mainGroup, m_rate, "sample-rate", 'r', "Audio samplerate");
  add(mainGroup, m_polyphony, "polyphony", 'p', "Number of voices");

  add(mainGroup, m_midiInputDeviceName, "midi-in", 'm', "Name of the alsa midi input device for raw midi input");
  add(mainGroup, m_tcdInputDeviceName, "tcd-in", 't', "Name of the alsa midi input device for tcd input");

  add(mainGroup, m_heartBeatDeviceName, "heartbeat", 'h',
      "Name of the alsa midi output device used to send heartbeats");
  add(mainGroup, m_audioOutputDeviceName, "audio-out", 'a', "Name of the alsa audio output device");
  add(mainGroup, m_fatalXRuns, "fatal-xruns", 'f', "Terminate program in case of alsa underrun or overrun");
  add(mainGroup, m_measurePerformance, "measure-performance", 'e', "Measure audio-engine realtime performance ");
  add(mainGroup, additionalMidiDelayString, "additional-midi-delay", 'i', "incoming midi note delay (in ns)");
  add(mainGroup, m_framesPerPeriod, "frames-per-period", 's', "alsa audio input frames per period");
  add(mainGroup, m_numPeriods, "num-periods", 'n', "alsa audio input number of periods");
  add(mainGroup, m_alsaBufferSize, "buffer-size", 'b', "alsa audio input ring buffer size");
  add(mainGroup, m_playgroundHost, "playground-host", 'x', "Where to find the playground");
  add(mainGroup, m_midiBridgeHost, "midi-bridge-host", 'd', "Where to find the midi bridge");
  add(mainGroup, m_cpuBurningSines, "num-sines", 'u',
      "Do not run the c15 synth, but run a lot of sines in order to burn CPU.");

  ctx.set_main_group(mainGroup);
  ctx.set_help_enabled(true);

  try
  {
    ctx.parse(argc, argv);
  }
  catch(const Glib::OptionError &optErr)
  {
    nltools::Log::error(optErr.what());
    std::exit(static_cast<int>(nltools::ErrorCode::UnknownOptionKeyError));
  }
  catch(const Glib::ConvertError &convErr)
  {
    nltools::Log::error(convErr.what());
    std::exit(static_cast<int>(nltools::ErrorCode::OptionConvertError));
  }
  catch(...)
  {
    std::stringstream ss;
    for(auto i = 0; i < argc; i++)
      ss << argv[i] << " ";
    nltools::Log::error(__FILE__, __FUNCTION__, __LINE__, "Could not parse args:", ss.str());
    std::exit(static_cast<int>(nltools::ErrorCode::UnknownOptionKeyError));
  }

  if(!additionalMidiDelayString.empty())
    m_additionalMidiDelay = std::chrono::nanoseconds(std::stoi(additionalMidiDelayString));
}

bool AudioEngineOptions::areXRunsFatal() const
{
  return m_fatalXRuns;
}

std::string AudioEngineOptions::getMidiInputDeviceName() const
{
  return m_midiInputDeviceName;
}

std::string AudioEngineOptions::getTcdInputDeviceName() const
{
  return m_tcdInputDeviceName;
}

std::chrono::nanoseconds AudioEngineOptions::getAdditionalMidiDelay() const
{
  return m_additionalMidiDelay;
}

std::string AudioEngineOptions::getHeartBeatDeviceName() const
{
  return m_heartBeatDeviceName;
}

std::string AudioEngineOptions::getAudioOutputDeviceName() const
{
  return m_audioOutputDeviceName;
}

int AudioEngineOptions::getFramesPerPeriod() const
{
  return m_framesPerPeriod;
}

int AudioEngineOptions::getNumPeriods() const
{
  return m_numPeriods;
}

int AudioEngineOptions::getAlsaRingBufferSize() const
{
  return m_alsaBufferSize;
}

std::string AudioEngineOptions::getPlaygroundHost() const
{
  return m_playgroundHost;
}

std::string AudioEngineOptions::getMidiBridgeHost() const
{
  return m_midiBridgeHost;
}

int AudioEngineOptions::getSampleRate() const
{
  return m_rate;
}

int AudioEngineOptions::getPolyphony() const
{
  return m_polyphony;
}

bool AudioEngineOptions::doMeasurePerformance()
{
  return m_measurePerformance;
}

int AudioEngineOptions::getNumCpuBurningSines() const
{
  return m_cpuBurningSines;
}