#include <io/framebuffer/FrameBufferSender.h>
#include <io/ToOledsBridge.h>
#include <nltools/messaging/Message.h>
#include <Application.h>
#include <BBBBOptions.h>
#include <glibmm/main.h>

ToOledsBridge::ToOledsBridge()
    : super(new FrameBufferSender(), nullptr)
{
  using namespace nltools::msg;
  receive<SetOLEDMessage>(EndPoint::Oled, sigc::mem_fun(this, &ToOledsBridge::onMessageReceived));
  receive<SetTimestampedOledMessage>(EndPoint::Oled, sigc::mem_fun(this, &ToOledsBridge::onTimestampedMessageRecieved));

  if(Application::get().getOptions()->doTimeStamps())
    Glib::MainContext::get_default()->signal_timeout().connect_seconds(
        sigc::mem_fun(this, &ToOledsBridge::printTurnaroundTime), 1);

  nltools::msg::send(nltools::msg::EndPoint::Playground, nltools::msg::OLEDState { 0 });
}

void ToOledsBridge::onMessageReceived(const nltools::msg::SetOLEDMessage &msg)
{
  if(auto fbs = static_cast<FrameBufferSender *>(m_sender.get()))
    fbs->send(msg.pixels, sizeof(msg.pixels));

  nltools::msg::send(nltools::msg::EndPoint::Playground, nltools::msg::OLEDState { msg.messageId });
}

void ToOledsBridge::onTimestampedMessageRecieved(const nltools::msg::SetTimestampedOledMessage &msg)
{
  onMessageReceived(msg.m_oledMessage);

  if(auto ms = msg.m_timestamp)
  {
    auto now = std::chrono::steady_clock::now();
    int64_t nowMS = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    auto diff = nowMS - ms;
    m_timeStamps.push_back({ now, diff });
  }
}

bool ToOledsBridge::printTurnaroundTime()
{
  removeOldRecords();

  auto maxIt = std::max_element(m_timeStamps.begin(), m_timeStamps.end(), [](auto a, auto b) { return a.ms < b.ms; });

  if(maxIt != m_timeStamps.end())
    nltools::Log::warning("Max turn around time in past second: ", maxIt->ms, "ms.");
  else
    nltools::Log::warning("No encoder event in past second.");

  return true;
}

void ToOledsBridge::removeOldRecords()
{
  auto now = std::chrono::steady_clock::now();

  auto e = std::remove_if(m_timeStamps.begin(), m_timeStamps.end(), [&](auto p) {
    auto age = now - p.captureTime;
    return age > std::chrono::seconds(1);
  });

  m_timeStamps.erase(e, m_timeStamps.end());
}
