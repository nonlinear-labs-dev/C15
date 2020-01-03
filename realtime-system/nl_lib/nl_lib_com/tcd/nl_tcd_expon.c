/*
 * nl_tcd_expon.c
 *
 *  Created on: 21.02.2015
 *      Author: ssc
 */

#include "math.h"

#include "nl_tcd_expon.h"

static uint32_t expTime[126];

// Der (exponentielle) Bereich von 104.0823997 dB (1 : 160000) wird auf eine (lineare) Skala von 0 ...16000 verteilt
// Das entspricht einer Differenz von 153.7243574 pro dB
// Für eine Halbierung der Zeit pro Oktave: 6.02 dB / 12 st = 925.5129 / 12 st = 77.126 pro semitone
// Für den Bereich (60 dB_T) der Attack/Release-Velocity: 9223.46

// Exp-Näherung durch eine Tabellen mit 126 Einträgen (125 Segmente für lineare Interpolation)
// ungefährer maximaler Fehler: 0.0012  = 1.2 promille

void EXPON_Init(void)
{
  float in;
  float time_ms;

  float logMin  = log(0.1);  // range limits [in ms] for Attack times
  float logMax  = log(16000);
  float logDiff = logMax - logMin;

  expTime[0] = 0;

  uint32_t i;

  for (i = 1; i < 126; i++)
  {
    in      = (float) i / 125.0;
    time_ms = exp(logMin + in * logDiff);

    expTime[i] = (uint32_t)(time_ms / TIME_UNIT_MS + 0.5);
  }
}

uint32_t EXPON_Time(int32_t paramVal)  // input range: 0 ... 16000; output range: 0.1 ... 16000 ms as a multiple of TIME_UNIT_MS
{
  if (paramVal <= 0)  // we can handle only positive values
  {
    return expTime[0];
  }
  else if (paramVal < 160)  // linear segment between 0.00 and 0.01
  {
    // return (uint32_t)(paramVal * (1.0 / 160.0) * expTime[1] + 0.5);  // (...) = 0.0339034
    return ((paramVal * expTime[1] * 13) >> 11);
  }
  else if (paramVal >= 16000)  // 16000 = 125 * 128
  {
    return expTime[125];
  }
  else
  {
    uint32_t index = paramVal >> 7;    // dividing by 128
    uint32_t fract = paramVal & 0x7f;  // 0...127

    return ((expTime[index] * (128 - fract) + expTime[index + 1] * fract) >> 7);  // linear interpolation for the lower 7 bits
  }
}
