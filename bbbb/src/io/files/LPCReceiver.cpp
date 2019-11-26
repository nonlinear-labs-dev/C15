#include <io/files/LPCReceiver.h>
#include "Application.h"
#include "Options.h"
#include "MessageParser.h"

LPCReceiver::LPCReceiver()
    : super("/dev/lpc_bb_driver", MessageParser::getNumInitialBytesNeeded())
    , m_parser(std::make_unique<MessageParser>())
{
}

LPCReceiver::~LPCReceiver()
{
}

void LPCReceiver::onDataReceived(Glib::RefPtr<Glib::Bytes> bytes)
{
  gsize numBytes = 0;
  auto data = reinterpret_cast<const uint8_t*>(bytes->get_data(numBytes));

  if(auto numBytesMissing = m_parser->parse(data, numBytes))
  {
    setBlockSize(numBytesMissing);
  }
  else
  {
    g_assert(m_parser->isFinished());

    auto msg = interceptHeartbeat(m_parser->getMessage());

    uint8_t b, *p;
    gsize len;
    p = (uint8_t *)(msg->get_data(len));
    for (int i=0; i<len; p++)
    {
        printf("%.2x ", *p);
    }

    super::onDataReceived(msg);
    m_parser = std::make_unique<MessageParser>();
    setBlockSize(MessageParser::getNumInitialBytesNeeded());
  }
}

Glib::RefPtr<Glib::Bytes> LPCReceiver::interceptHeartbeat(Glib::RefPtr<Glib::Bytes> msg)
{
  constexpr auto heartbeatMessageType = 0x0B00;
  constexpr auto heartbeatMessageHeaderSize = 4;
  constexpr auto heartbeatMessagePayloadSize = 8;
  constexpr auto heartbeatMessageSize = heartbeatMessageHeaderSize + heartbeatMessagePayloadSize;

  gsize msgLength = 0;
  auto rawMsg = msg->get_data(msgLength);
  auto rawBytes = reinterpret_cast<const uint8_t*>(rawMsg);
  auto rawWords = reinterpret_cast<const uint16_t*>(rawBytes);

  if(msgLength == heartbeatMessageSize && rawWords[0] == heartbeatMessageType)
  {
    auto lpcHeartBeatPtr = reinterpret_cast<const uint64_t*>(rawBytes + heartbeatMessageHeaderSize);
    auto lpcHeartBeat = *lpcHeartBeatPtr;
    auto chainHeartBeat = lpcHeartBeat + m_heartbeat;

    uint8_t scratch[heartbeatMessageSize];
    memcpy(scratch, rawBytes, heartbeatMessageSize);
    memcpy(scratch + heartbeatMessageHeaderSize, &chainHeartBeat, heartbeatMessagePayloadSize);

    return Glib::Bytes::create(scratch, heartbeatMessageSize);
  }

  return msg;
}
