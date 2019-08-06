#pragma once

#include "soled/SOLED.h"

class Application;

class PlayPanel
{
 public:
  void onTimeout();
  SOLED &getSOLED();

 private:
  SOLED m_oled;
};
