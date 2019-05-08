#include "MessageParser.h"
#include "testing/TestDriver.h"
#include <string.h>
#include "device-settings/DebugLevel.h"

static TestDriver<MessageParser> tests;

MessageParser::NLMessage::NLMessage()
    : type(0)
    , length(0)
{
}

MessageParser::NLMessage::~NLMessage()
{
}

void MessageParser::NLMessage::trace(const char *fromLocation)
{
  DebugLevel::info("Tracing message from", fromLocation);

  DebugLevel::info("-> type =", type);
  DebugLevel::info("-> length =", length);

  for(int i = 0; i < params.size(); i++)
  {
    DebugLevel::info("-> param", i, "=", params[i]);
  }
}

MessageParser::MessageParser()
    : m_numBytesRead(0)
{
}

MessageParser::~MessageParser()
{
}

bool MessageParser::hasReadHeader() const
{
  return m_numBytesRead >= getNumInitialBytesNeeded();
}

size_t MessageParser::getNumHeaderBytesMissing() const
{
  return getNumInitialBytesNeeded() - m_numBytesRead;
}

size_t MessageParser::parse(const uint8_t *buffer, size_t numBytes)
{
  if(hasReadHeader())
    return parseBody(buffer, numBytes);

  size_t headerBytesMissing = getNumHeaderBytesMissing();
  size_t missingBytesAvail = std::min(headerBytesMissing, numBytes);

  uint8_t *rawMsg = reinterpret_cast<uint8_t *>(&m_msg);
  memcpy(rawMsg + m_numBytesRead, buffer, missingBytesAvail);

  buffer += missingBytesAvail;
  numBytes -= missingBytesAvail;
  m_numBytesRead += missingBytesAvail;

  if(m_numBytesRead == getNumInitialBytesNeeded())
  {
    m_msg.params.resize(m_msg.length, 0);
  }

  if(m_numBytesRead >= getNumInitialBytesNeeded())
    return parseBody(buffer, numBytes);

  return getNumInitialBytesNeeded() - m_numBytesRead;
}

size_t MessageParser::parseBody(const uint8_t *buffer, size_t numBytes)
{
  size_t numBytesInBody = m_msg.length * sizeof(uint16_t);
  size_t numBytesToReadAtAll = getNumInitialBytesNeeded() + numBytesInBody;

  if(numBytesToReadAtAll > m_numBytesRead)
  {
    size_t bytesToReadNow = std::min(numBytes, numBytesToReadAtAll - m_numBytesRead);
    size_t posInBody = m_numBytesRead - getNumInitialBytesNeeded();
    uint8_t *rawMsgBody = reinterpret_cast<uint8_t *>(m_msg.params.data());
    memcpy(rawMsgBody + posInBody, buffer, bytesToReadNow);
    m_numBytesRead += bytesToReadNow;
  }

  size_t stillToDo = numBytesToReadAtAll - m_numBytesRead;

  if(stillToDo == 0)
    m_msg.trace(G_STRLOC);

  return stillToDo;
}

size_t MessageParser::getNumInitialBytesNeeded()
{
  return sizeof(NLMessage::type) + sizeof(NLMessage::length);
}

const MessageParser::NLMessage &MessageParser::getMessage() const
{
  return std::move(m_msg);
}

void MessageParser::registerTests()
{
  g_test_add_func("/MessageParser/stream", []() {
    uint16_t data[] = { PRESET_DIRECT, 2, 99, 77 };
    uint8_t *byteData = reinterpret_cast<uint8_t *>(data);

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
          g_assert(msg.type == PRESET_DIRECT);
          g_assert(msg.length == 2);
          g_assert(msg.params[0] == 99);
          g_assert(msg.params[1] == 77);
          break;
        }

        todo -= todoNow;
        done += todoNow;
      }
    }
  });
}
