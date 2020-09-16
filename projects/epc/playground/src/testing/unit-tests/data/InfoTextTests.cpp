#include <testing/TestHelper.h>
#include <proxies/hwui/panel-unit/boled/setup/SetupInfoTexts.h>

TEST_CASE("Map has all Info Texts")
{
  for(auto& e : getAllValues<SetupInfoEntries>())
  {
    try
    {
      SetupInfoContent.at(e);
    }
    catch(...)
    {
      FAIL(toString(e) + " not Found in Content Map!");
    }

    try
    {
      SetupInfoHeaders.at(e);
    }
    catch(...)
    {
      FAIL(toString(e) + " not Found in Header Map!");
    }
  }
}