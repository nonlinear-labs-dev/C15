#include "TestDriver.h"
#include "device-settings/DebugLevel.h"

static TestDriverBase *listOfDrivers = nullptr;
static bool s_running = false;

TestDriverBase::TestDriverBase()
    : next(nullptr)
{
#ifdef _TESTS
  next = listOfDrivers;
  listOfDrivers = this;
#endif
}

TestDriverBase::~TestDriverBase()
{
}

bool TestDriverBase::isInTestRun()
{
  return s_running;
}

void TestDriverBase::doTests(int &numArgs, char **&argv)
{
#ifdef _TESTS
  g_test_init(&numArgs, &argv, NULL);

  DebugLevel::info("Running tests:");

  TestDriverBase *foo = listOfDrivers;

  while(foo)
  {
    foo->registerTests();
    foo = foo->next;
  }

  s_running = true;
  if(g_test_run() == 0)
    DebugLevel::info("All tests ran without errors.");
  else
    DebugLevel::info("!!! There have been errors running the tests !!!");
  s_running = false;

  g_log_set_default_handler(g_log_default_handler, nullptr);
#endif
}
