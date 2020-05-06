#pragma once

#include "lpc-defs.h"

// ===================
typedef struct
{
  const char *const            name;
  const char *const            help;
  const EHC_ControllerConfig_T config;         // .ctrlId = 1 means "ring active"
  const int                    rangeMin;       // -1 means do not set
  const int                    rangeMax;       // -1 means do not set
  const int8_t                 deadZoneLower;  // -1 means do not set
  const int8_t                 deadZoneUpper;  // -1 means do not set
} EHC_PresetT;

extern const EHC_PresetT EHC_presets[];
extern const unsigned    EHC_NUMBER_OF_PRESETS;
