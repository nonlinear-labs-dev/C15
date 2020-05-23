#pragma once

#include <stdint.h>
#include "lpc-defs.h"

static inline uint16_t EHC_configToUint16(const EHC_ControllerConfig_T c)
{
  uint16_t ret = 0;
  ret |= c.autoHoldStrength << 0;
  ret |= c.continuous << 3;
  ret |= c.doAutoRanging << 4;
  ret |= c.polarityInvert << 5;
  ret |= c.pullup << 6;
  ret |= c.is3wire << 7;
  ret |= c.ctrlId << 8;
  ret |= c.silent << 11;
  ret |= c.hwId << 12;
  return ret;
}

static inline EHC_ControllerConfig_T EHC_uint16ToConfig(const uint16_t c)
{
  EHC_ControllerConfig_T ret;
  ret.autoHoldStrength = (c & 0b0000000000000111) >> 0;
  ret.continuous       = (c & 0b0000000000001000) >> 3;
  ret.doAutoRanging    = (c & 0b0000000000010000) >> 4;
  ret.polarityInvert   = (c & 0b0000000000100000) >> 5;
  ret.pullup           = (c & 0b0000000001000000) >> 6;
  ret.is3wire          = (c & 0b0000000010000000) >> 7;
  ret.ctrlId           = (c & 0b0000011100000000) >> 8;
  ret.silent           = (c & 0b0000100000000000) >> 11;
  ret.hwId             = (c & 0b1111000000000000) >> 12;
  return ret;
}

static inline uint16_t EHC_statusToUint16(const EHC_ControllerStatus_T s)
{
  uint16_t ret = 0;
  ret |= s.initialized << 0;
  ret |= s.pluggedIn << 1;
  ret |= s.isReset << 2;
  ret |= s.outputIsValid << 3;
  ret |= s.isAutoRanged << 4;
  ret |= s.isSettled << 5;
  ret |= s.isRamping << 6;
  ret |= s.isSaved << 7;
  ret |= s.isRestored << 8;
  return ret;
}

static inline EHC_ControllerStatus_T EHC_uint16ToStatus(const uint16_t s)
{
  EHC_ControllerStatus_T ret;
  ret.initialized   = (s & 0b0000000000000001) >> 0;
  ret.pluggedIn     = (s & 0b0000000000000010) >> 1;
  ret.isReset       = (s & 0b0000000000000100) >> 2;
  ret.outputIsValid = (s & 0b0000000000001000) >> 3;
  ret.isAutoRanged  = (s & 0b0000000000010000) >> 4;
  ret.isSettled     = (s & 0b0000000000100000) >> 5;
  ret.isRamping     = (s & 0b0000000001000000) >> 6;
  ret.isSaved       = (s & 0b0000000010000000) >> 7;
  ret.isRestored    = (s & 0b0000000100000000) >> 8;
  return ret;
}
