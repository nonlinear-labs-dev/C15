#include "CommandLineInterface.h"
#include <nltools/logging/Log.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>

int kbhit()
{
  termios old_termios;
  termios new_termios;

  tcgetattr(0, &old_termios);
  new_termios = old_termios;
  cfmakeraw(&new_termios);
  tcsetattr(0, TCSANOW, &new_termios);

  struct timeval tv = { 0L, 0L };
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(0, &fds);
  int r = select(1, &fds, nullptr, nullptr, &tv);
  tcsetattr(0, TCSANOW, &old_termios);
  return r;
}

char getch()
{
  return static_cast<char>(getchar());
}

CommandLineInterface::CommandLineInterface()
{
  if(isatty(fileno(stdin)))
    Glib::signal_timeout().connect(sigc::mem_fun(this, &CommandLineInterface::onIO), 100);
}

CommandLineInterface::~CommandLineInterface() = default;

bool CommandLineInterface::onIO()
{
  if(kbhit())
    this->processByte(getch());

  return true;
}
