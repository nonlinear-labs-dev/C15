#include <glibmm/error.h>
#include <glibmm/spawn.h>
#include <nltools/system/SpawnCommandLine.h>
#include <nltools/logging/Log.h>

SpawnCommandLine::SpawnCommandLine(const std::string &cmd)
{
  try
  {
    Glib::spawn_command_line_sync(cmd, &m_stdOutput, &m_stdError, &m_exitStatus);
  }
  catch(Glib::Error &error)
  {
    nltools::Log::warning(error.what());
  }
}

std::string SpawnCommandLine::getStdOutputOrFallback(const std::string &fallbackIfEmpty) const
{
  return m_stdOutput.empty() ? fallbackIfEmpty : m_stdOutput;
}

const std::string &SpawnCommandLine::getStdOutput() const
{
  return m_stdOutput;
}

const std::string &SpawnCommandLine::getStdError() const
{
  return m_stdError;
}

int SpawnCommandLine::getExitStatus() const
{
  return m_exitStatus;
}
