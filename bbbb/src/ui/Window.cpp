#ifdef _DEVELOPMENT_PC

#include <Application.h>
#include <nltools/messaging/Message.h>
#include <ui/Window.h>
#include <io/FromButtonsBridge.h>
#include <io/Bridges.h>

class WebSocketReceiver;

constexpr auto framebufferDimX = 256;
constexpr auto framebufferDimY = 96;

Window::Window()
{
  set_default_size(framebufferDimX, framebufferDimY * 2);
  set_size_request(framebufferDimX, framebufferDimY * 2);

  using namespace nltools::msg;

  receive<SetOLEDMessage>(EndPoint::Oled, sigc::mem_fun(this, &Window::onFrameBufferMessageReceived));
  receive<SetTimestampedOledMessage>(EndPoint::Oled,
                                     sigc::mem_fun(this, &Window::onFrameBufferTimestampedMessageReceived));
  receive<SetPanelLEDMessage>(EndPoint::PanelLed, sigc::mem_fun(this, &Window::onPanelLEDsMessageReceived));

  m_ribbonUp.set_vexpand(false);
  m_ribbonDown.set_vexpand(false);
  m_ribbonBox.pack_start(m_ribbonUp, false, false);
  m_ribbonBox.pack_end(m_ribbonDown, false, false);
  m_ribbonBox.set_homogeneous(true);
  m_box.pack_start(m_playPanel, true, true);
  m_box.pack_end(m_editPanel, false, false);
  m_box.pack_start(m_ribbonBox, false, false);
  //m_box.pack_end(m_keyboard, false, false);
  add(m_box);

  show_all_children(true);
}

Window::~Window()
{
}

void Window::onFrameBufferMessageReceived(const nltools::msg::SetOLEDMessage &msg)
{
  m_playPanel.setFrameBuffer(msg);
}

void Window::onPanelLEDsMessageReceived(const nltools::msg::SetPanelLEDMessage &msg)
{
  m_editPanel.setLed(msg.id, msg.on);
}

void Window::onFrameBufferTimestampedMessageReceived(const nltools::msg::SetTimestampedOledMessage &msg)
{
  m_playPanel.setFrameBuffer(msg.m_oledMessage);
}

#endif
