#include "MessageParser.h"
#include <string.h>

MessageParser::MessageParser()
{
}

MessageParser::~MessageParser()
{
}

bool MessageParser::hasReadHeader() const
{
  return m_numBytesRead >= getNumInitialBytesNeeded();
}

gsize MessageParser::getNumHeaderBytesMissing() const
{
  return getNumInitialBytesNeeded() - m_numBytesRead;
}

gsize MessageParser::parse(gconstpointer buffer, gsize numBytes)
{
  if(!hasReadHeader())
  {
    if(auto missingForHeader = parseHeader(buffer, numBytes))
    {
      return missingForHeader;
    }

    return parseBody(nullptr, 0);
  }

  return parseBody(buffer, numBytes);
}

gsize MessageParser::parseHeader(gconstpointer buffer, gsize numBytes)
{
  if(hasReadHeader())
    return 0;

  gsize headerBytesMissing = getNumHeaderBytesMissing();
  gsize missingBytesAvail = std::min(headerBytesMissing, numBytes);

  uint8_t *rawMsg = reinterpret_cast<uint8_t *>(&m_header);
  memcpy(rawMsg + m_numBytesRead, buffer, missingBytesAvail);
  m_numBytesRead += missingBytesAvail;

  if(m_numBytesRead == getNumInitialBytesNeeded())
  {
    m_params.resize(m_header.length, 0);
  }

  auto stillMissing = headerBytesMissing - missingBytesAvail;
  return stillMissing;
}

gsize MessageParser::parseBody(gconstpointer buffer, gsize numBytes)
{
  gsize numBytesToReadAtAll = getMessageLength();
  gsize missingBytes = numBytesToReadAtAll - m_numBytesRead;
  gsize missingBytesAvail = std::min(missingBytes, numBytes);

  if(buffer)
  {
    gsize posInBody = m_numBytesRead - getNumInitialBytesNeeded();
    uint8_t *rawMsgBody = reinterpret_cast<uint8_t *>(m_params.data());
    memcpy(rawMsgBody + posInBody, buffer, missingBytesAvail);
    m_numBytesRead += missingBytesAvail;
  }

  auto stillMissing = missingBytes - missingBytesAvail;
  return stillMissing;
}

gsize MessageParser::getNumInitialBytesNeeded()
{
  return sizeof(NLMessageHeader);
}

Glib::RefPtr<Glib::Bytes> MessageParser::getMessage() const
{
  auto len = getMessageLength();
  uint8_t buf[len];

  memcpy(buf, &m_header, sizeof(NLMessageHeader));
  memcpy(buf + sizeof(NLMessageHeader), m_params.data(), 2 * m_header.length);

  return Glib::Bytes::create(buf, len);
}

gsize MessageParser::getMessageLength() const
{
  g_assert(hasReadHeader());
  gsize numBytesInBody = m_header.length * sizeof(uint16_t);
  gsize numBytesToReadAtAll = getNumInitialBytesNeeded() + numBytesInBody;
  return numBytesToReadAtAll;
}

bool MessageParser::isFinished() const
{
  if(!hasReadHeader())
    return false;

  gsize numBytesToReadAtAll = getMessageLength();
  return m_numBytesRead >= numBytesToReadAtAll;
}
