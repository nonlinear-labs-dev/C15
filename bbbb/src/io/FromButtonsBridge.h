#pragma once

#include <io/Bridge.h>

class FromButtonsBridge : public Bridge
{
 public:
  FromButtonsBridge();
  void sendKey(int8_t key, bool down);

  void transmit(Receiver::tMessage msg) override;
};
