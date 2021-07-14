#include "Font.h"
#include "FrameBuffer.h"
#include "Commands.h"

char* selfPath = nullptr;

int main(int numArgs, char** argv)
{
  Gio::init();

  selfPath = argv[0];

  Commands commands;

  if(numArgs > 1)
  {
    commands.execute(numArgs - 1, argv + 1);
  }
  else
  {
    std::string line;

    while(std::getline(std::cin, line))
    {
      auto args = g_strsplit((const gchar*) line.c_str(), (const gchar*) " ", -1);
      auto argsLen = g_strv_length(args);
      commands.execute(argsLen, args);
      g_strfreev(args);
    }
  }

  sync();
  return 0;
}
