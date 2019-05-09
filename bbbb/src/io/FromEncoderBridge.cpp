#include <io/files/FileIOReceiver.h>
#include <io/FromEncoderBridge.h>
#include <io/network/WebSocketSender.h>
#include <Application.h>
#include <Options.h>
#include <string.h>

FromEncoderBridge::FromEncoderBridge()
    : Bridge(new WebSocketSender(getDomain()), new FileIOReceiver("/dev/espi_encoder", 1))
{
}

FromEncoderBridge::~FromEncoderBridge()
{
}

Domain FromEncoderBridge::getDomain()
{
  return Application::get().getOptions()->doTimeStamps() ? Domain::TimeStampedRotary : Domain::Rotary;
}

void FromEncoderBridge::transmit(Receiver::tMessage msg)
{
  gsize numBytes = 0;
  auto data = static_cast<const int8_t*>(msg->get_data(numBytes));
  sendRotary(data[0]);
}

void FromEncoderBridge::sendRotary(int8_t inc)
{
  if(Application::get().getOptions()->doTimeStamps())
    scheduleTimestampedEvent(inc);
  else
    scheduleSimpleEvent(inc);
}

void FromEncoderBridge::scheduleTimestampedEvent(int8_t inc)
{
  if(m_firstPendingEventTime == std::chrono::steady_clock::time_point::min())
    m_firstPendingEventTime = std::chrono::steady_clock::now();

  auto timeStamp = std::exchange(m_firstPendingEventTime, std::chrono::steady_clock::time_point::min());
  int64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(timeStamp.time_since_epoch()).count();
  uint8_t data[9];
  memcpy(data, &ms, 8);
  memcpy(data + 8, &inc, 1);
  auto msg = Glib::Bytes::create(data, 9);
  m_sender->send(msg);
}

void FromEncoderBridge::scheduleSimpleEvent(int8_t inc)
{
  auto msg = Glib::Bytes::create(&inc, 1);
  m_sender->send(msg);
}
