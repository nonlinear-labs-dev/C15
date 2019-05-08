#pragma once

#include <glib/gerror.h>
#include <glibmm/ustring.h>
#include <string>

class SpawnCommandLine
{
 public:
  SpawnCommandLine(const std::string &cmd);

  std::string getStdOutputOrFallback(const std::string &fallbackIfEmpty) const;
  const std::string &getStdOutput() const;
  const std::string &getStdError() const;

 private:
  std::string m_stdOutput;
  std::string m_stdError;
};
