#include <io/ToRibbonLEDsBridge.h>
#include "files/FileIOSender.h"

#include <nltools/messaging/Message.h>

ToRibbonLEDsBridge::ToRibbonLEDsBridge()
    : super(new FileIOSender("/dev/ribbon_led"), nullptr)
{

  using namespace nltools::msg;
  receive<SetRibbonLEDMessage>(EndPoint::RibbonLed, sigc::mem_fun(this, &ToRibbonLEDsBridge::onMessageReceived));
}

void ToRibbonLEDsBridge::onMessageReceived(const nltools::msg::SetRibbonLEDMessage &msg)
{
  uint8_t data[2];
  data[0] = msg.id;
  data[1] = msg.brightness;
  static_cast<FileIOSender *>(m_sender.get())->write(reinterpret_cast<const char *>(data), 2);
}
