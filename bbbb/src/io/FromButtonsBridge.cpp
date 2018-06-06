#include <io/files/FileIOReceiver.h>
#include <io/FromButtonsBridge.h>
#include <io/network/WebSocketSender.h>

FromButtonsBridge::FromButtonsBridge() :
    Bridge(new WebSocketSender(Domain::Buttons),
           new FileIOReceiver("/dev/espi_buttons", 1))
{
}

FromButtonsBridge::~FromButtonsBridge()
{
}

void FromButtonsBridge::sendKey(int key, bool down)
{
  auto data = new int8_t[1];
  *data = key;
  if(down)
    *data |= 1 << 7;

  auto msg = Glib::Bytes::create(data,1);
  m_sender->send(msg);
}