#pragma once

#include "proxies/hwui/FourStateLED.h"
#include "parameters/Parameter.h"

#define NUM_LEDS_PER_RIBBON 33

class Application;

class Ribbon
{
 public:
  Ribbon();
  virtual ~Ribbon();

  void syncBBBB();

 protected:
  void initLEDs();
  void setLEDState(int ledPos, char state, bool flush);
  void resetLEDs(bool flush);
  void setLEDsForValueUniPolar(tDisplayValue paramVal, bool flush);
  void setLEDsUniPolar(int paramValIdx, bool flush);
  void setLEDsForValueBiPolar(tDisplayValue paramVal, bool flush);
  void setLEDsBiPolar(int paramValIdx, int direction, bool flush);

  virtual int posToLedID(int pos) const = 0;

 private:
  FourStateLED m_leds[NUM_LEDS_PER_RIBBON];
};
