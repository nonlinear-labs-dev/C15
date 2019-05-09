#include <io/files/FileIOReceiver.h>
#include <io/FromButtonsBridge.h>
#include <io/network/WebSocketSender.h>

FromButtonsBridge::FromButtonsBridge()
    : Bridge(new WebSocketSender(Domain::Buttons), new FileIOReceiver("/dev/espi_buttons", 1))
{
}

FromButtonsBridge::~FromButtonsBridge()
{
}

void FromButtonsBridge::sendKey(int key, bool down)
{
  int8_t data[1];
  *data = static_cast<int8_t>(key);

  if(down)
    *data |= 1 << 7;

  auto msg = Glib::Bytes::create(data, 1);
  m_sender->send(msg);
}
