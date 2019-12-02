// BBB communication
// KSTR 2019-11-04

#ifndef __COMM_H__
#define __COMM_H__

#include <stdint.h>

void COMM_Init(void);  // 300 bps, 8 bits, 2 stop bits, odd parity
void COMM_DeInit(void);

void COMM_StartStatusWrite(void);
void COMM_ProccessReadCommands(void);
void COMM_ProccessWriteStatus(void);

#endif

// end of file
