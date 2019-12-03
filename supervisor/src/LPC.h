// LPC processing (muting communication)
// KSTR 2019-11-01

#ifndef __LPC_H__
#define __LPC_H__

#include <stdint.h>
#include "globals.h"

void LPC_Start_MonitorAudioEngineStatus(int32_t timeout);
void LPC_Stop_MonitorAudioEngineStatus(void);
void LPC_MonitorAudioEngineStatus_Process(void);

#endif

// end of file