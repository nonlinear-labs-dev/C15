#include "Ribbon.h"
#include <glib.h>
#include <glib.h>
#include <typeinfo>
#include <math.h>
#include "device-settings/DebugLevel.h"
#include <thread>

Ribbon::Ribbon()
{
}

Ribbon::~Ribbon()
{
  resetLEDs(true);
}

void Ribbon::syncBBBB()
{
  for(int i = 0; i < NUM_LEDS_PER_RIBBON; i++)
  {
    m_leds[i].syncBBBB();
  }
}

void Ribbon::initLEDs()
{
  for(int i = 0; i < NUM_LEDS_PER_RIBBON; i++)
  {
    int id = posToLedID(i);
    m_leds[i].setID(id);
  }

  resetLEDs(true);
}

void Ribbon::setLEDState(int ledPos, char state, bool flush)
{
  m_leds[ledPos].setState(state, flush);

#if _DEVELOPMENT_PC && 0
  Glib::ustring dbg;

  for(int i = 0; i < NUM_LEDS_PER_RIBBON; i++)
  {
    char s = m_leds[i].getState();
    if(s == 0)
      dbg += '-';
    else if(s == 1)
      dbg += '.';
    else if(s == 2)
      dbg += 'o';
    else if(s == 3)
      dbg += 'O';
  }

  DebugLevel::warning("Ribbon:", typeid(*this).name(), dbg.c_str());
#endif
}

void Ribbon::resetLEDs(bool flush)
{
  for(int i = 0; i < NUM_LEDS_PER_RIBBON; i++)
    setLEDState(i, 0, flush);
}

void Ribbon::setLEDsForValueUniPolar(tDisplayValue paramVal, bool flush)
{
  int numRepresentableStates = NUM_LEDS_PER_RIBBON * 3 + 1;
  int paramValIdx = lround(paramVal * numRepresentableStates);
  setLEDsUniPolar(paramValIdx, flush);
}

void Ribbon::setLEDsUniPolar(int paramValIdx, bool flush)
{
  int incomingParamVal = paramValIdx;

  for(int i = 0; i < NUM_LEDS_PER_RIBBON; i++)
  {
    int brightness = 0;

    for(int b = 0; b < 3; b++)
    {
      paramValIdx -= 1;

      if(paramValIdx > 0)
        brightness++;
    }

    if(i == 0 && incomingParamVal > 0)
      brightness = std::max(brightness, 1);

    setLEDState(i, brightness, flush);
  }
}

void Ribbon::setLEDsForValueBiPolar(tDisplayValue paramVal, bool flush)
{
  resetLEDs(false);

  if(paramVal != 0.0)
  {
    int numRepresentableStates = (3 * (NUM_LEDS_PER_RIBBON - 1) / 2);
    int paramValIdx = lround(paramVal * numRepresentableStates);
    setLEDsBiPolar(paramValIdx, (paramVal >= 0) ? 1 : -1, flush);
  }

  setLEDState(NUM_LEDS_PER_RIBBON / 2, 3, flush);
}

void Ribbon::setLEDsBiPolar(int paramValIdx, int direction, bool flush)
{
  resetLEDs(flush);

  int idx = (NUM_LEDS_PER_RIBBON - 1) / 2 + direction;

  paramValIdx = abs(paramValIdx);

  for(int i = 0; i < NUM_LEDS_PER_RIBBON / 2; i++)
  {
    int brightness = 0;

    for(int b = 0; b < 3; b++)
    {
      if(paramValIdx > 0)
        brightness++;

      paramValIdx -= 1;
    }

    if(i == 0)
      brightness = std::max(brightness, 1);

    setLEDState(idx, brightness, flush);
    idx += direction;
  }
}
