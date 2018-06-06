#pragma once

#include <io/Bridge.h>

class FromLPCBridge : public Bridge
{
  public:
    FromLPCBridge();
    virtual ~FromLPCBridge();
    void sendRibbonPosition(bool m_upperRibon, double value);

};

