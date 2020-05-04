#define CATCH_CONFIG_RUNNER
#include <catch.hpp>
#include <nltools/logging/Log.h>
#include <giomm.h>

int main(int numArgs, char **argv)
{
  Gio::init();
  nltools::Log::setLevel(nltools::Log::Error);
  return Catch::Session().run(numArgs, argv);
}
