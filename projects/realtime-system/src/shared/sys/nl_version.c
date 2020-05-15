/******************************************************************************/
/** @file		nl_version.h
    @date		2020-04-24
    @version	0
    @author		KSTR
    @brief		define firmware version and place string in image
*******************************************************************************/
#include "sys/nl_version.h"

#define STR_IMPL_(x) #x            //stringify argument
#define STR(x)       STR_IMPL_(x)  //indirection to expand argument macros
#ifdef CORE_M4
#define CORE "M4"
#else
#define CORE "M0"
#endif

const char VERSION_STRING[] = "\n\nC15 RT-SYS, LPC4337 Core " CORE ", FIRMWARE VERSION: " STR(SW_VERSION) " \n\n\0\0\0";

void* GetVersionString()
{
  return (void*) VERSION_STRING;
}
