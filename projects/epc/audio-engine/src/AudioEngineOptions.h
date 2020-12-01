#pragma once

#include <chrono>
#include <glibmm/ustring.h>

class AudioEngineOptions
{
 public:
  AudioEngineOptions(int &argc, char **&argv);

  bool areXRunsFatal() const;
  bool isExternalMidiEnabled() const;
  int getSampleRate() const;
  int getPolyphony() const;
  bool doMeasurePerformance();

  // alsa midi
  std::string getMidiInputDeviceName() const;
  std::string getTcdInputDeviceName() const;
  std::chrono::nanoseconds getAdditionalMidiDelay() const;
  std::string getHeartBeatDeviceName() const;

  // alsa audio
  std::string getAudioOutputDeviceName() const;
  int getFramesPerPeriod() const;
  int getNumPeriods() const;
  int getAlsaRingBufferSize() const;

  std::string getPlaygroundHost() const;
  std::string getMidiBridgeHost() const;
  int getNumCpuBurningSines() const;

 private:
  bool m_fatalXRuns = false;
  int m_rate = 48000;
  int m_polyphony = 20;
  bool m_measurePerformance = false;

  Glib::ustring m_playgroundHost = "localhost";
  Glib::ustring m_midiBridgeHost = "192.168.10.11";

  Glib::ustring m_midiInputDeviceName;
  Glib::ustring m_tcdInputDeviceName;

  std::chrono::nanoseconds m_additionalMidiDelay = std::chrono::nanoseconds::zero();
  Glib::ustring m_heartBeatDeviceName;

  Glib::ustring m_audioOutputDeviceName;
  int m_framesPerPeriod = 96;
  int m_numPeriods = 2;
  int m_alsaBufferSize = 96 * 2;
  int m_cpuBurningSines = 0;
};
