#pragma once

class AudioOutput;

class CommandlinePerformanceWatch
{
 public:
  CommandlinePerformanceWatch(AudioOutput *device);

 private:
  bool printPerformance();

  AudioOutput *m_device;
};
