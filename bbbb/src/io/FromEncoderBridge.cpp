#include <io/files/FileIOReceiver.h>
#include <io/FromEncoderBridge.h>
#include <Application.h>
#include <Options.h>
#include <string.h>
#include <nltools/messaging/Messaging.h>
#include <nltools/messaging/Message.h>

FromEncoderBridge::FromEncoderBridge()
    : Bridge(nullptr, new FileIOReceiver("/dev/espi_encoder", 1))
{
}

void FromEncoderBridge::transmit(Receiver::tMessage msg)
{
  gsize numBytes = 0;
  auto data = static_cast<const int8_t*>(msg->get_data(numBytes));
  sendRotary(data[0]);
}

void FromEncoderBridge::sendRotary(int8_t inc)
{
  nltools::msg::RotaryChangedMessage msg;
  msg.increment = inc;
  nltools::msg::send(nltools::msg::EndPoint::Playground, msg);
}
