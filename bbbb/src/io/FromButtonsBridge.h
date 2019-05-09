#pragma once

#include <io/Bridge.h>

class FromButtonsBridge : public Bridge
{
 public:
  FromButtonsBridge();
  virtual ~FromButtonsBridge();
  void sendKey(int key, bool down);
};
