#pragma once

/******************************************************************************/
/**	@file       c15_config.h
    @date       2020-02-03, 19:44
    @version	1.7B-3
    @author     M. Seeber
    @brief      crucial information about the audio engine
    @todo
*******************************************************************************/

#include <stdint.h>

namespace C15
{

  namespace Config
  {

    const char milestone[7] = "1.7B-3";
    const uint32_t total_polyphony = 24;
    const uint32_t local_polyphony = total_polyphony >> 1;
    const uint32_t tcd_elements = 400;
    const uint32_t clock_rates[2][3] = { { 48000, 9600, 400 }, { 96000, 9600, 400 } };
    const float fade_time_ms = 3.0f;
    const uint32_t key_count = 61;
    const uint32_t key_center = 24;
    const uint32_t key_reference = 33;
    const uint32_t key_from = 36;
    const uint32_t key_to = 96;

  }  // namespace C15::Config

}  // namespace C15
