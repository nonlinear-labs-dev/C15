#pragma once

#include <io/Bridge.h>

class ToLPCBridge : public Bridge
{
    using super = Bridge;

  public:
    ToLPCBridge();
    virtual ~ToLPCBridge();
};

