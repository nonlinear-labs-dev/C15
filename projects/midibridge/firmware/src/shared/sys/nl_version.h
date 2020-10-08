/******************************************************************************/
/** @file		nl_version.h
    @date		2020-04-24
    @version	0
    @author		KSTR
    @brief		define firmware version and place string in image
*******************************************************************************/
#pragma once

// ==== V 205 ====
// fix for max. Velocity und max. HWSource, added keybed press sent to BBBB
// Velocity-Auflösung 14 Bit statt 12 Bit
// #define SW_VERSION 205

// ==== V 206 ====
// complete revised "pedals" (aka external controllers) proccessing
// 58 == release candidate, beta test
// 59 == release candidate, beta test
// 60 == release candidate, beta test, lots of improvements (Keybed Scanner)
// 61 == release candidate, beta test, removed all standard libraries, and now using hardware floating point
// 62 == release candidate, beta test, introduce jitter on ESPI and keybed scanner for EMC, add key mapping
// 63 == release candidate, rewritten MIDI buffering
#define SW_VERSION 206

#define STR_IMPL_(x) #x            //stringify argument
#define STR(x)       STR_IMPL_(x)  //indirection to expand argument macros
#if defined CORE_M4
#define CORE "M4"
#elif defined CORE_M0
#define CORE "M0"
#else
#error "either CORE_M4 or CORE_M0 must be defined!"
#endif

static const char VERSION_STRING[] = "\n\nC15 RT-SYS, LPC4337 Core " CORE ", FIRMWARE VERSION: " STR(SW_VERSION) " \n\n\0\0\0";
