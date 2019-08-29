#include <io/ToLPCBridge.h>
#include <Application.h>
#include "files/FileIOSender.h"
#include <nltools/messaging/Message.h>

ToLPCBridge::ToLPCBridge()
    : super(new FileIOSender("/dev/lpc_bb_driver"), nullptr)
{

  nltools::msg::receive<nltools::msg::LPCMessage>(nltools::msg::EndPoint::Lpc,
                                                  sigc::mem_fun(this, &ToLPCBridge::onMessage));
}

void ToLPCBridge::onMessage(const nltools::msg::LPCMessage &msg)
{
  gsize numBytes = 0;
  auto data = reinterpret_cast<const char *>(msg.message->get_data(numBytes));
  std::cerr << "received ToLPCBridge Message!" << std::endl;
  static_cast<FileIOSender *>(m_sender.get())->write(data, numBytes);
}
