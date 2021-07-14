#include <testing/TestHelper.h>
#include <glibmm.h>
#include <xml/XmlReader.h>
#include <xml/MemoryInStream.h>
#include <serialization/Serializer.h>
#include <CompileTimeOptions.h>
#include <xml/FileOutStream.h>

TEST_CASE("Glib::Markup::escape_text() newline")
{
  auto val = Glib::Markup::escape_text("\n");
  CHECK(val == "\n");
}

TEST_CASE("XML Reader Trim")
{
  auto trim = XmlReader::trim;
  CHECK(trim(" ").empty());
  CHECK(trim("\n ").empty());
  CHECK(trim("  \n\t \t ").empty());
  CHECK(trim(" \n  \t abc \n\t\n ") == "abc");
  CHECK(trim(" \n\t abcf s \n \t as \n\t  ") == "abcf s \n \t as");
  CHECK(trim("INIT für Comb Sound") == "INIT für Comb Sound");
  CHECK(trim("Z1\nZ2\nZ3\n") == "Z1\nZ2\nZ3");
}

class MockSerializer : public Serializer
{
 public:
  MockSerializer()
      : Serializer(getTagName())
  {
  }

  static Glib::ustring getTagName()
  {
    return "a";
  }

 protected:
  void writeTagContent(Writer &writer) const override
  {
  }
  void readTagContent(Reader &reader) const override
  {
    reader.onTextElement("b", [](auto str, const Attributes &attr) { CHECK(str == "Z1\nZ2"); });
  }
};

TEST_CASE("XML Reader Test - Memory In Stream")
{
  Glib::ustring testXML = "<a><b>Z1\nZ2</b></a>";
  MemoryInStream stream(testXML, false);
  auto scope = TestHelper::createTestScope();
  XmlReader reader(stream, scope->getTransaction());
  reader.read<MockSerializer>();
}

TEST_CASE("XML Reader Test - File In Stream")
{
  const auto testFilePath
      = "/tmp/" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".xml";

  {
    FileOutStream stream(testFilePath, false);
    stream.write("<a><b>Z1\nZ2</b></a>");
    stream.commit();
  }

  FileInStream stream(testFilePath, false);
  auto scope = TestHelper::createTestScope();
  XmlReader reader(stream, scope->getTransaction());
  reader.read<MockSerializer>();
}