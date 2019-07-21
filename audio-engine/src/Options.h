#pragma once

#include <glibmm.h>

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

  // alsa audio
  std::string getAudioOutputDeviceName() const;
  int getFramesPerPeriod() const;
  int getNumPeriods() const;
  int getAlsaRingBufferSize() const;

  std::string getPlaygroundHost() const;

 private:
  bool m_fatalXRuns = false;
  int m_rate = 48000;
  int m_polyphony = 20;
  bool m_measurePerformance = false;

  Glib::ustring m_playgroundHost = "localhost";
  Glib::ustring m_midiInputDeviceName;
  std::chrono::nanoseconds m_additionalMidiDelay = std::chrono::nanoseconds::zero();

  Glib::ustring m_audioOutputDeviceName;
  int m_framesPerPeriod = 96;
  int m_numPeriods = 2;
  int m_alsaBufferSize = 96 * 2;
};
