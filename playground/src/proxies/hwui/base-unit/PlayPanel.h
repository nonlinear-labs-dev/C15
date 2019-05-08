#pragma once

#include "soled/SOLED.h"

class Application;

class PlayPanel
{
 public:
  PlayPanel();
  virtual ~PlayPanel();

  void onTimeout();
  SOLED &getSOLED();

 private:
  SOLED m_oled;
};
