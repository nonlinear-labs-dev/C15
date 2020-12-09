#pragma once

/******************************************************************************/
/**	@file       c15_config.h
    @date       2020-12-05, 14:53
    @version    1.7B-8
    @author     M. Seeber
    @brief      crucial information about the audio engine
    @todo
*******************************************************************************/

#include <stdint.h>

namespace C15
{

  namespace Config
  {

    const char milestone[7] = "1.7B-8";
    constexpr uint32_t total_polyphony = 24;
    constexpr uint32_t local_polyphony = total_polyphony >> 1;
    constexpr uint32_t tcd_elements = 400;
    const uint32_t clock_rates[2][3] = { { 48000, 9600, 400 }, { 96000, 9600, 400 } };
    constexpr float fade_time_ms = 3.0f;
    // C15 physical key range: lowest, highest, count
    constexpr uint32_t physical_key_from = 36;
    constexpr uint32_t physical_key_to = 96;
    constexpr uint32_t physical_key_count = 1 + physical_key_to - physical_key_from;
    // C15 virtual key range: lowest, highest, count (fully midi compatible)
    constexpr uint32_t virtual_key_from = 0;
    constexpr uint32_t virtual_key_to = 127;
    constexpr uint32_t virtual_key_count = 1 + virtual_key_to - virtual_key_from;
    // generic special keys (absolute pivot point for key tracking, relative center for panning, relative reference for tuning)
    constexpr uint32_t generic_key_pivot = 60;
    constexpr uint32_t offset_key_center = 6;
    constexpr uint32_t offset_key_reference = 9;

  }  // namespace C15::Config

}  // namespace C15
