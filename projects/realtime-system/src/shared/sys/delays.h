/******************************************************************************/
/** @file		delays.h
    @date		2015-02-12 dtz
    @author		Daniel Tzschentke 2015-02-12
*******************************************************************************/
#ifndef DELAYS_H_
#define DELAYS_H_

#define DELAY_ONE_CLK_CYCLE asm volatile("nop")  // 204 MHz => ~4.9ns

#define DELAY_TEN_CLK_CYCLES \
  DELAY_ONE_CLK_CYCLE;       \
  DELAY_ONE_CLK_CYCLE;       \
  DELAY_ONE_CLK_CYCLE;       \
  DELAY_ONE_CLK_CYCLE;       \
  DELAY_ONE_CLK_CYCLE;       \
  DELAY_ONE_CLK_CYCLE;       \
  DELAY_ONE_CLK_CYCLE;       \
  DELAY_ONE_CLK_CYCLE;       \
  DELAY_ONE_CLK_CYCLE;       \
  DELAY_ONE_CLK_CYCLE;

#define DELAY_HUNDRED_CLK_CYCLES \
  DELAY_TEN_CLK_CYCLES           \
  DELAY_TEN_CLK_CYCLES           \
  DELAY_TEN_CLK_CYCLES           \
  DELAY_TEN_CLK_CYCLES           \
  DELAY_TEN_CLK_CYCLES           \
  DELAY_TEN_CLK_CYCLES           \
  DELAY_TEN_CLK_CYCLES           \
  DELAY_TEN_CLK_CYCLES           \
  DELAY_TEN_CLK_CYCLES           \
  DELAY_TEN_CLK_CYCLES

#define DELAY_THOUSAND_CLK_CYCLES \
  DELAY_HUNDRED_CLK_CYCLES        \
  DELAY_HUNDRED_CLK_CYCLES        \
  DELAY_HUNDRED_CLK_CYCLES        \
  DELAY_HUNDRED_CLK_CYCLES        \
  DELAY_HUNDRED_CLK_CYCLES        \
  DELAY_HUNDRED_CLK_CYCLES        \
  DELAY_HUNDRED_CLK_CYCLES        \
  DELAY_HUNDRED_CLK_CYCLES        \
  DELAY_HUNDRED_CLK_CYCLES        \
  DELAY_HUNDRED_CLK_CYCLES

#endif
