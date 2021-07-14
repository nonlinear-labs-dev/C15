#pragma once

#include <string>

class Command
{
 public:
  Command();
  virtual ~Command();

  virtual int getNumArguments() const = 0;
  virtual void execute(char** argv, int numArgs) = 0;
  virtual std::string getHelp() const = 0;
  virtual std::string getName() const = 0;
};
