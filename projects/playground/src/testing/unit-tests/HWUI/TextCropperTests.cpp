#include <testing/TestHelper.h>
#include <CompileTimeOptions.h>
#include <proxies/hwui/Font.h>
#include <proxies/hwui/TextCropper.h>

TEST_CASE("Shorten String if Necessary", "[HWUI][Text]")
{
  auto pathToFont = getResourcesDir() + "/Emphase-9-Regular.ttf";
  auto font = std::make_shared<Font>(pathToFont, 9);

  auto testShortenStringIfNecessary = [font](const std::string& in, int len, const std::string& expected) {
    auto shortened = TextCropper::shortenStringIfNeccessary(font, in, len);
    CHECK(shortened == expected);
  };

  testShortenStringIfNecessary("", 5, "");
  testShortenStringIfNecessary("", 0, "");
  testShortenStringIfNecessary("", 100000, "");

  testShortenStringIfNecessary("Fo", 5, "..");
  testShortenStringIfNecessary("Fo", 20, "Fo");
  testShortenStringIfNecessary("Fooo", 20, "Fooo");
  testShortenStringIfNecessary("Foooo", 20, "F..o");
  testShortenStringIfNecessary("Foooo 2", 20, "F..2");
  testShortenStringIfNecessary("Foooo2", 20, "F..2");

  testShortenStringIfNecessary("ANANAS SANANA", 20, "A..A");
  testShortenStringIfNecessary("ANANAS SANANA", 40, "ANA..ANA");
  testShortenStringIfNecessary("ANANAS SANANA 4", 40, "ANAN..4");
  testShortenStringIfNecessary("ANANAS SANANA 4 5", 40, "ANAN..5");
  testShortenStringIfNecessary("ANANASSANANA 2", 60, "ANANASSA..2");

  testShortenStringIfNecessary("HALLOHALLOHALLOHALLOHALLO 78", 128, "HALLOHALLOHALLOHAL..78");
  testShortenStringIfNecessary("HALLO HALLO HALLO HALLO HALLO 78", 128, "HALLO HALLO HALLO HA..78");
}