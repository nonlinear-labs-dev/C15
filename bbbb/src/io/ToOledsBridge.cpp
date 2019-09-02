#include <io/framebuffer/FrameBufferSender.h>
#include <io/ToOledsBridge.h>
#include <nltools/messaging/Message.h>

ToOledsBridge::ToOledsBridge()
    : super(new FrameBufferSender(), nullptr)
{
  using namespace nltools::msg;
  receive<SetOLEDMessage>(EndPoint::Oled, sigc::mem_fun(this, &ToOledsBridge::onMessageReceived));
}

void ToOledsBridge::onMessageReceived(const nltools::msg::SetOLEDMessage &msg)
{
  if(auto fbs = static_cast<FrameBufferSender *>(m_sender.get())) {
    fbs->send(msg.pixels, sizeof(msg.pixels));
  }
}
