#pragma once

#include "playground.h"
#include <set>

class TestDriverBase
{
 public:
  TestDriverBase();
  virtual ~TestDriverBase();

  static void doTests(int &numArgs, char **&argv);
  static bool isInTestRun();

 private:
  virtual void registerTests() = 0;
  TestDriverBase *next;
};

template <typename tClassToTest> class TestDriver : public TestDriverBase
{
 public:
  TestDriver()
  {
  }
  virtual ~TestDriver()
  {
  }

 private:
  void registerTests()
  {
    tClassToTest::registerTests();
  }
};
