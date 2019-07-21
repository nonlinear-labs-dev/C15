#pragma once

#include <io/Bridge.h>
#include <nltools/threading/Throttler.h>
#include <list>

class FromEncoderBridge : public Bridge
{
 public:
  FromEncoderBridge();
  void sendRotary(int8_t inc);

 private:
  void transmit(Receiver::tMessage msg) override;
};
