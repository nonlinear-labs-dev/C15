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

void sendAssertionToBB(const char *ptr)
{
  int len = strlen(ptr);
  len     = MIN(len, 500);

  uint16_t *dataPtr = (uint16_t *) ptr;

  BB_MSG_WriteMessage(BB_MSG_TYPE_ASSERTION, len / 2, dataPtr);

  //schedule harakiri
  //COOS_Task_Add(HardFault_Handler, 16000, 0);
}
