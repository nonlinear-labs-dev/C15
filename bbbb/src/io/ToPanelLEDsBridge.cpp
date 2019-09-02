#include <io/ToPanelLEDsBridge.h>
#include "files/FileIOSender.h"

#include <nltools/messaging/Message.h>

ToPanelLEDsBridge::ToPanelLEDsBridge()
    : super(new FileIOSender("/dev/espi_led"), nullptr)
{
  using namespace nltools::msg;
  receive<SetPanelLEDMessage>(EndPoint::PanelLed, sigc::mem_fun(this, &ToPanelLEDsBridge::onMessageReceived));
}

void ToPanelLEDsBridge::onMessageReceived(const nltools::msg::SetPanelLEDMessage &msg)
{
  uint8_t val = (static_cast<uint8_t>((msg.on ? 1 : 0) << 7)) | (msg.id & 0x7F);
  static_cast<FileIOSender *>(m_sender.get())->write(reinterpret_cast<const char *>(&val), 1);
}
