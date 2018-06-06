#pragma once

#include <io/Bridge.h>

class FromEncoderBridge : public Bridge
{
  public:
    FromEncoderBridge();
    virtual ~FromEncoderBridge();

    void sendRotary(int8_t inc);

};

