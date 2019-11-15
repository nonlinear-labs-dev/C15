#pragma once

#include <glibmm.h>
#include "synth/c15-audio-engine/pe_defines_config.h"

class Options
{
 public:
  Options(int &argc, char **&argv);

  bool areXRunsFatal() const;
  int getSampleRate() const;
  int getPolyphony() const;
  bool doMeasurePerformance();

  // alsa midi
  std::string getMidiInputDeviceName() const;
  std::chrono::nanoseconds getAdditionalMidiDelay() const;
  std::string getHeartBeatDeviceName() const;

  // alsa audio
  std::string getAudioOutputDeviceName() const;
  int getFramesPerPeriod() const;
  int getNumPeriods() const;
  int getAlsaRingBufferSize() const;

 private:
  bool m_fatalXRuns = false;
  int m_rate = 48000;
  int m_polyphony = dsp_number_of_voices;
  bool m_measurePerformance = false;

  Glib::ustring m_midiInputDeviceName;
  std::chrono::nanoseconds m_additionalMidiDelay = std::chrono::nanoseconds::zero();
  Glib::ustring m_heartBeatDeviceName;

  Glib::ustring m_audioOutputDeviceName;
  int m_framesPerPeriod = 96;
  int m_numPeriods = 2;
  int m_alsaBufferSize = 96 * 2;
};
