/******************************************************************************/
/** @file		nl_assert.h
    @date		2016-04-20 DTZ
    @author		2016-04-20 DTZ
*******************************************************************************/

#include "nl_assert.h"
#include "spibb/nl_bb_msg.h"
#include <string.h>
#include "cmsis/lpc_types.h"
#include "sys/nl_coos.h"
#include <stdio.h>

void sendAssertionToBB(const char *ptrFunc, int line)
{
#if 1
  char txt[512];
  sprintf(txt, "%s:%d", ptrFunc, line);
  uint16_t len = strlen(txt);
  len          = MIN(len, 500);

  BB_MSG_WriteMessage(BB_MSG_TYPE_ASSERTION, len / 2, (uint16_t *) txt);
  BB_MSG_SendTheBuffer();

#else
  uint16_t data[4] = { 0 };
  data[0]          = 0x55;
  data[1]          = 0xAA;
  data[2]          = 0xBB;
  data[3]          = 0xFF;

  BB_MSG_WriteMessage2Arg(BB_MSG_TYPE_ASSERTION, 122, 123);
  BB_MSG_SendTheBuffer();
#endif
  //schedule harakiri
  //COOS_Task_Add(HardFault_Handler, 16000, 0);
}
