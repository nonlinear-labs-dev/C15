#include <io/files/LPCReceiver.h>
#include "Application.h"
#include "Options.h"
#include "MessageParser.h"
#include <string.h>
#include <cstdio>
#include <cstdlib>
#include <bitset>
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

  static constexpr uint16_t BB_MSG_TYPE_SENSORS_RAW = 0x0E00;

  void logMessage(const Glib::RefPtr<Glib::Bytes> &bytes)
  {
    static uint32_t cntr = 0;
    gsize msgLength = 0;
    auto rawMsg = bytes->get_data(msgLength);
    auto rawWords = reinterpret_cast<const uint16_t *>(rawMsg);

    switch(rawWords[0])
    {
      case BB_MSG_TYPE_SENSORS_RAW:  // all raw sensor values
      {
        if(rawWords[1] != 13)
          return;
        printf("BB_MSG_TYPE_SENSORS_RAW: ");
        // Pedal detect bits (Pedal_4...Pedal_1) :
        std::bitset<4> binbits(rawWords[2] & 0x000F);
        std::cout << binbits;
        // Pedal ADC values (TIP1 and RING1 ... TIP4 and RING4, four pairs) :
        printf(" %4d %4d %4d %4d %4d %4d %4d %4d ", rawWords[3], rawWords[4], rawWords[5], rawWords[6], rawWords[7],
               rawWords[8], rawWords[9], rawWords[10]);
        // Pitchbender, Aftertouch, Ribbon 1, Ribbon 2 :
        printf("%4d %4d %4d %4d", rawWords[11], rawWords[12], rawWords[13], rawWords[14]);
        printf("\n\033[1A");  // send "cursor up one line"
        fflush(stdout);       // flush output, so that piping updates nicely when used via SSH
        break;
      }
    }
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