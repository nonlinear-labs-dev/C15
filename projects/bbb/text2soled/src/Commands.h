#pragma once

#include <list>
#include <string>

class Command;
class Commands
{
 public:
  Commands();
  virtual ~Commands();

  void execute(int numArgs, char *argv[]);

 private:
  Command *findCommand(const std::string &name) const;
  void showHelp();
  void executeCommand(Command *c, int numArgs, char **argv);

  std::list<Command *> m_commands;
};
