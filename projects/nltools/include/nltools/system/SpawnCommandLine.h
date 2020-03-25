#pragma once
#include <string>

class SpawnCommandLine
{
 public:
  explicit SpawnCommandLine(const std::string &cmd);

  std::string getStdOutputOrFallback(const std::string &fallbackIfEmpty) const;
  const std::string &getStdOutput() const;
  const std::string &getStdError() const;
  int getExitStatus() const;

 private:
  std::string m_stdOutput;
  std::string m_stdError;
  int m_exitStatus {};
};
