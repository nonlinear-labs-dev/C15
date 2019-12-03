/*
 * nl_tcd_test.c
 *
 *  Created on: 28.03.2015
 *      Author: ssc
 */

#include "nl_tcd_msg.h"

void TEST_Output(uint32_t param, int32_t value)
{
#if 0
	MSG_SelectParameter(500 + param);		/// war bisher 400, kollidiert nun mit verschobenen Audio-Param-Ids
	MSG_SetDestinationSigned(value);
#endif
}
