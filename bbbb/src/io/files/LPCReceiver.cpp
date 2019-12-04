#include <io/files/LPCReceiver.h>
#include "Application.h"
#include "Options.h"
#include "MessageParser.h"
#include <string.h>
#include <iomanip>
#include <iterator>

LPCReceiver::LPCReceiver()
    : super("/dev/lpc_bb_driver", MessageParser::getNumInitialBytesNeeded())
    , m_parser(std::make_unique<MessageParser>())
{
}

LPCReceiver::~LPCReceiver() = default;

namespace Heartbeat
{
  constexpr auto messageType = 0x0B00;
  constexpr auto headerSize = 4;
  constexpr auto payloadSize = 8;
  constexpr auto messageSize = headerSize + payloadSize;
}

namespace Log
{
  void logMessage(const Glib::RefPtr<Glib::Bytes> &bytes)
  {
    static uint32_t cntr=0;
    gsize msgLength = 0;
    auto rawMsg = bytes->get_data(msgLength);
    auto rawWords = reinterpret_cast<const uint16_t *>(rawMsg);

    switch (rawWords[0])
    {
    case 0xFF01 :  // raw ribbon values, single line live screen output version
      printf("Ribbon Raw Data (%.8x): %5d  %5d \n\033[1A", cntr++, rawWords[2], rawWords[3]);
      break;
    case 0xFF02 :  // raw ribbon values, standard output (suitable for piping/redirection)
      printf("Ribbon Raw Data (%.8x): %5d  %5d \n\033[1A", cntr++, rawWords[2], rawWords[3]);
      break;
    }

/*
    for(gsize i = 0; i < msgLength/2; i++)
    {
      printf("%.4x ", rawWords[i]);
    }
    if (msgLength >= 6)
      printf("  (%d)", (int16_t)rawWords[(msgLength/2)-1]);
    std::cout << '\n';
*/
  }

  void logHeartbeat(const char *desc, const Glib::RefPtr<Glib::Bytes> &bytes)
  {
    gsize msgLength = 0;
    auto rawMsg = bytes->get_data(msgLength);
    auto rawBytes = reinterpret_cast<const uint8_t *>(rawMsg);
    auto rawWords = reinterpret_cast<const uint16_t *>(rawBytes);

    const auto msgType = rawWords[0];

    if(msgLength == Heartbeat::messageSize && msgType == Heartbeat::messageType)
    {
      auto lpcHeartBeatPtr = reinterpret_cast<const uint64_t *>(rawBytes + Heartbeat::headerSize);
      auto lpcHeartBeat = *lpcHeartBeatPtr;
      std::cout << desc << std::hex << lpcHeartBeat << '\n';
    }
  }
}
void LPCReceiver::onDataReceived(Glib::RefPtr<Glib::Bytes> bytes)
{
  const static bool logLPCRaw = Application::get().getOptions()->logLPCRaw();
  const static bool logHeartbeat = Application::get().getOptions()->logHeartBeat();

  gsize numBytes = 0;
  auto data = reinterpret_cast<const uint8_t *>(bytes->get_data(numBytes));

  if(auto numBytesMissing = m_parser->parse(data, numBytes))
  {
    setBlockSize(numBytesMissing);
  }
  else
  {
    g_assert(m_parser->isFinished());

    auto message = m_parser->getMessage();

    if(logLPCRaw)
      Log::logMessage(message);
    if(logHeartbeat)
      Log::logHeartbeat("lpc heartbeat:\t", message);

    message = interceptHeartbeat(message);

    super::onDataReceived(message);
    m_parser = std::make_unique<MessageParser>();
    setBlockSize(MessageParser::getNumInitialBytesNeeded());
  }
}

Glib::RefPtr<Glib::Bytes> LPCReceiver::interceptHeartbeat(Glib::RefPtr<Glib::Bytes> msg)
{
  gsize msgLength = 0;
  auto rawMsg = msg->get_data(msgLength);
  auto rawBytes = reinterpret_cast<const uint8_t *>(rawMsg);
  auto rawWords = reinterpret_cast<const uint16_t *>(rawBytes);

  if(msgLength == Heartbeat::messageSize && rawWords[0] == Heartbeat::messageType)
  {
    auto lpcHeartBeatPtr = reinterpret_cast<const uint64_t *>(rawBytes + Heartbeat::headerSize);
    auto lpcHeartBeat = *lpcHeartBeatPtr;
    auto chainHeartBeat = lpcHeartBeat + m_heartbeat;

    uint8_t scratch[Heartbeat::messageSize];
    memcpy(scratch, rawBytes, Heartbeat::messageSize);
    memcpy(scratch + Heartbeat::headerSize, &chainHeartBeat, Heartbeat::payloadSize);

    return Glib::Bytes::create(scratch, Heartbeat::messageSize);
  }

  return msg;
}
