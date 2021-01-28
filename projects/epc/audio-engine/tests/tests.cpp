#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

int main(int numArgs, char **argv)
{
  return Catch::Session().run(numArgs, argv);
}