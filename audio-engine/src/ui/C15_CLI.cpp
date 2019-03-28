#include "C15_CLI.h"
#include "main.h"
#include "synth/C15Synth.h"
#include "io/Log.h"
#include <iostream>

C15_CLI::C15_CLI(C15Synth *synth)
{
  m_commands['c'] = [=] { synth->printAndResetTcdInputLog(); };
  m_commands['e'] = [=] { synth->resetDSP(); };
  m_commands['t'] = [=] { synth->toggleTestTone(); };
  m_commands['z'] = [=] { synth->selectTestToneFrequency(); };
  m_commands['u'] = [=] { synth->selectTestToneAmplitude(); };
  m_commands['r'] = [=] { synth->resetPerformance(); };
  m_commands['+'] = [=] { synth->increase(); };
  m_commands['-'] = [=] { synth->decrease(); };

  m_commands['q'] = [=] {
    Log::info("Quitting application...");
    quit(0);
  };
}

void C15_CLI::processByte(char c)
{
  auto it = m_commands.find(c);
  if(it != m_commands.end())
    it->second();
}
