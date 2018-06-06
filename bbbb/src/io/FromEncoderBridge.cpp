#include <io/files/FileIOReceiver.h>
#include <io/FromEncoderBridge.h>
#include <io/network/WebSocketSender.h>

FromEncoderBridge::FromEncoderBridge() :
    Bridge(new WebSocketSender(Domain::Rotary),
           new FileIOReceiver("/dev/espi_encoder", 1))
{
}

FromEncoderBridge::~FromEncoderBridge()
{
}

void FromEncoderBridge::sendRotary(int8_t inc)
{
  auto msg = Glib::Bytes::create(g_memdup(&inc, 1), 1);
  m_sender->send(msg);
}
