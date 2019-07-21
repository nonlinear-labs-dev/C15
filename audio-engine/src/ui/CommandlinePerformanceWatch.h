#pragma once

class AudioOutput;

class CommandlinePerformanceWatch
{
 public:
  CommandlinePerformanceWatch(const AudioOutput *device);

 private:
  bool printPerformance();

  const AudioOutput *m_device;
};
