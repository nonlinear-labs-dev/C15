/******************************************************************************/
/** @file		tcd_decoder.h
    @date       2018-08-21
    @version    1.0
    @author     Matthias Seeber
    @brief		TCD Decoder object evaluating incoming TCD messages
*******************************************************************************/

#pragma once

#include <stdint.h>
#include "pe_utilities.h"
#include "pe_defines_protocol.h"
#include "pe_defines_lists.h"

struct decoder
{
  /* local variables */

  uint32_t m_voiceFrom;  // TCD Voice selection: start id memory
  uint32_t m_voiceTo;    // TCD Voice selection: end id memory
  uint32_t m_paramFrom;  // TCD Parameter selection: start id memory
  uint32_t m_paramTo;    // TCD Parameter selection: end id memory

  uint32_t m_value;  // temporary value memory
  int32_t m_sign;    // temporary sign memory

  uint32_t m_utilityId = 0;  // TCD Utility selection: utility id memory
  uint32_t m_listId = 0;     // TCD List selection: list id memory
  uint32_t m_listIndex = 0;  // TCD List traversal: index memory

  /* local data structures */

  const int32_t m_getSign[2] = { 1, -1 };  // TCD Sign evaluation
  int32_t m_event[5];                      // TCD Selection Event

  dual_id_list<uint32_t> m_selectedVoices;                       // ID List of selected Voices
  polyDual_id_list<Parameters> m_selectedParams;             // ID List of selected Parameters
  id_list<Parameters> m_listTraversal[lst_number_of_lists];  // ID Lists for Preset Recall and Key Events

  /* decoder functions */

  void init();                                    // proper Init
  uint32_t getCommandId(const uint32_t _status);  // TCD command evaluation

  uint32_t unsigned14(const uint32_t _data0, const uint32_t _data1);  // TCD argument parsing: unsigned 14 bit values
  int32_t signed14(const uint32_t _data0, const uint32_t _data1);     // TCD argument parsing: signed 14 bit values
  void unsigned28upper(const uint32_t _data0,
                       const uint32_t _data1);  // TCD argument parsing: unsigned 28 bit values (upper 14 bit)
  void signed28upper(const uint32_t _data0,
                     const uint32_t _data1);  // TCD argument parsing: signed 28 bit values (upper 14 bit)
  int32_t apply28lower(const uint32_t _data0,
                       const uint32_t _data1);  // TCD argument parsing: 28 bit values (lower 14 bit)

  uint32_t selectionEvent(const uint32_t _from, const uint32_t _to,
                          const uint32_t _id);  // TCD Voice and Parameter selection event evaluation

  Parameters traverseRecall();    // TCD List traversal: Preset Recall Events
  Parameters traverseKeyEvent();  // TCD List traversal: Key Events
};
