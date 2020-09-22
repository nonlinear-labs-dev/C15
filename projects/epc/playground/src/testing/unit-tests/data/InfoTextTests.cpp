#include <testing/TestHelper.h>
#include <proxies/hwui/panel-unit/boled/setup/SetupInfoTexts.h>

TEST_CASE("Map has all Info Texts")
{
  for(auto& e : getAllValues<SetupInfoEntries>())
  {
    try
    {
      SetupInfoTexts::getInfoContent(e);
    }
    catch(...)
    {
      FAIL(toString(e) + " not Found in Content Map!");
    }

    try
    {
      SetupInfoTexts::getInfoHeader(e);
    }
    catch(...)
    {
      FAIL(toString(e) + " not Found in Header Map!");
    }
  }
}