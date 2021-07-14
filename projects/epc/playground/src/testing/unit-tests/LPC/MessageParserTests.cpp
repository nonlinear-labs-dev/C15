#include <catch.hpp>
#include <proxies/playcontroller/MessageParser.h>

TEST_CASE("Message Parser")
{
  SECTION("stream")
  {
    uint16_t data[] = { MessageParser::PRESET_DIRECT, 2, 99, 77 };
    auto *byteData = reinterpret_cast<uint8_t *>(data);

    size_t msgLen = sizeof(data);

    for(size_t chunksize = 1; chunksize < msgLen; chunksize++)
    {
      MessageParser foo;

      size_t todo = msgLen;
      size_t done = 0;

      while(todo)
      {
        size_t todoNow = std::min(todo, chunksize);
        size_t needed = foo.parse(byteData + done, todoNow);

        if(needed == 0)
        {
          const MessageParser::NLMessage &msg = foo.getMessage();
          CHECK(msg.type == MessageParser::PRESET_DIRECT);
          CHECK(msg.length == 2);
          CHECK(msg.params[0] == 99);
          CHECK(msg.params[1] == 77);
          break;
        }

        todo -= todoNow;
        done += todoNow;
      }
    }
  }
}
