#include "nltools/system/SpawnAsyncCommandLine.h"
#include <algorithm>

std::list<std::unique_ptr<AsyncCommandLine>> SpawnAsyncCommandLine::m_commands;

bool SpawnAsyncCommandLine::spawn(const std::vector<std::string>& command,
                                  std::function<void(const std::string&)> success,
                                  std::function<void(const std::string&)> error)
{
  m_commands.emplace_back(std::make_unique<AsyncCommandLine>(command, success, error));
  removeDone();
  return true;
}

size_t SpawnAsyncCommandLine::getNumCommands()
{
  return m_commands.size();
}

size_t SpawnAsyncCommandLine::removeDone()
{
  auto size = m_commands.size();
  m_commands.remove_if([](auto& c) { return !c->isRunning(); });
  return size - m_commands.size();
}
