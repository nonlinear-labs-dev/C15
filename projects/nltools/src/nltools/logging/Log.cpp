#include <nltools/logging/Log.h>
#include <iostream>
#include <mutex>

nltools::Log::Level nltools::Log::s_level = nltools::Log::Level::Debug;

void nltools::Log::flush()
{
  std::cout << std::flush;
}

void nltools::Log::print(const std::string &str)
{
  static std::mutex m;
  std::unique_lock<std::mutex> l(m);
  std::cout << str;
}

void nltools::Log::printNewLine()
{
  static std::mutex m;
  std::unique_lock<std::mutex> l(m);
  std::cout << std::endl;
}
