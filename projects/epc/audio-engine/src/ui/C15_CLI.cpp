#include "C15_CLI.h"
#include "synth/C15Synth.h"
#include "io/audio/AudioOutput.h"
#include <nltools/logging/Log.h>
#include <iostream>

C15_CLI::C15_CLI(C15Synth *synth, AudioOutput *audioOut)
{
  using namespace std::chrono_literals;

  m_commands['c'] = [=] { synth->logStatus(); };
  m_commands['e'] = [=] { synth->resetDSP(); };
  m_commands['t'] = [=] { synth->toggleTestTone(); };
  m_commands['z'] = [=] { synth->selectTestToneFrequency(); };
  m_commands['u'] = [=] { synth->selectTestToneAmplitude(); };

  m_commands['r'] = [=] {
    if(audioOut)
      audioOut->resetPerformance();
  };

  m_commands['+'] = [=] { synth->increase(); };
  m_commands['-'] = [=] { synth->decrease(); };

  m_commands['q'] = [=] {
    nltools::Log::info("Quitting application...");
    exit(0);
  };
}

void C15_CLI::processByte(char c)
{
  auto it = m_commands.find(c);
  if(it != m_commands.end())
    it->second();
}
