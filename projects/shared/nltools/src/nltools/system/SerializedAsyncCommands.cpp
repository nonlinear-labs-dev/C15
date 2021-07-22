#include "nltools/system/SerializedAsyncCommands.h"
#include "nltools/system/AsyncCommandLine.h"

SerializedAsyncCommands::SerializedAsyncCommands() = default;
SerializedAsyncCommands::~SerializedAsyncCommands() = default;

void SerializedAsyncCommands::schedule(const Command &command, const SuccessCB &onSuccess, const ErrorCB &onError)
{
  m_records.push_back({ command, onSuccess, onError });

  if(!m_currentCommand)
    doCommand();
}

void SerializedAsyncCommands::doCommand()
{
  m_currentCommand.reset();

  if(!m_records.empty())
  {
    auto cmd = std::move(m_records.front());
    m_records.pop_front();

    m_currentCommand = std::make_unique<AsyncCommandLine>(cmd.cmd,
                                                          [this, success = std::move(cmd.success)](const std::string& a) {
                                                            success(a);
                                                            doCommand();
                                                          },
                                                          [this, error = std::move(cmd.error)](const std::string& a) {
                                                            error(a);
                                                            doCommand();
                                                          });
  }
}
