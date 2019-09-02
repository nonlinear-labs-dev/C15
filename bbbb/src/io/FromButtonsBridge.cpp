#include <io/files/FileIOReceiver.h>
#include <io/FromButtonsBridge.h>
#include <nltools/messaging/Message.h>

FromButtonsBridge::FromButtonsBridge()
    : Bridge(nullptr, new FileIOReceiver("/dev/espi_buttons", 1))
{
}

void FromButtonsBridge::sendKey(int8_t key, bool down)
{
  nltools::msg::ButtonChangedMessage msg;
  msg.buttonId = key;
  msg.pressed = down;
  nltools::msg::send(nltools::msg::EndPoint::Playground, msg);
}

void FromButtonsBridge::transmit(Receiver::tMessage msg)
{
  gsize numBytes = 0;
  auto buffer = (const char *) msg->get_data(numBytes);

  if(numBytes > 0)
    sendKey(buffer[0] & 0x7F, buffer[0] & 0x80);
}
