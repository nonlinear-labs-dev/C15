#include <serialization/RecallEditBufferSerializer.h>
#include <xml/FileOutStream.h>
#include <presets/EditBuffer.h>
#include <catch.hpp>
#include "testing/TestHelper.h"

TEST_CASE("Recall write and Read")
{
  auto eb = TestHelper::getEditBuffer();

  RecallEditBufferSerializer serializer(eb);

  auto ebHashBefore = eb->getHash();

  SECTION("a")
  {
    FileOutStream fos("/tmp/recalldata.xml", false);

    XmlWriter writer { fos };
    serializer.write(writer);
    fos.commit();

    {
      auto fis = std::make_shared<FileInStream>("/tmp/recalldata.xml", false);
      auto scope = TestHelper::createTestScope();
      XmlReader reader { *fis.get(), scope->getTransaction() };
      serializer.read(reader);
    }

    auto ebAfter = eb->getHash();
    REQUIRE(ebHashBefore == ebAfter);
  }
}
