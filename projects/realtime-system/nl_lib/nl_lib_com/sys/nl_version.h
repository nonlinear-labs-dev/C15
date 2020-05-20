/******************************************************************************/
/** @file		nl_version.h
    @date		2020-04-24
    @version	0
    @author		KSTR
    @brief		define firmware version and place string in image
*******************************************************************************/
#ifndef NL_VERSION_H_
#define NL_VERSION_H_

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
#define SW_VERSION 62206

void* GetVersionString();

#endif
