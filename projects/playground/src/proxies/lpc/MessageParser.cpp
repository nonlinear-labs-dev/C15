#include "MessageParser.h"
#include <string.h>
#include "device-settings/DebugLevel.h"

MessageParser::NLMessage::NLMessage()
    : type(0)
    , length(0)
{
}

MessageParser::NLMessage::~NLMessage() = default;

MessageParser::MessageParser()
    : m_numBytesRead(0)
{
}

MessageParser::~MessageParser() = default;

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

  auto *rawMsg = reinterpret_cast<uint8_t *>(&m_msg);
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
    auto *rawMsgBody = reinterpret_cast<uint8_t *>(m_msg.params.data());
    memcpy(rawMsgBody + posInBody, buffer, bytesToReadNow);
    m_numBytesRead += bytesToReadNow;
  }

  size_t stillToDo = numBytesToReadAtAll - m_numBytesRead;

  return stillToDo;
}

size_t MessageParser::getNumInitialBytesNeeded()
{
  return sizeof(NLMessage::type) + sizeof(NLMessage::length);
}

const MessageParser::NLMessage &MessageParser::getMessage() const
{
  return m_msg;
}