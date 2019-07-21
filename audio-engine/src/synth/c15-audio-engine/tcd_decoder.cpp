/******************************************************************************/
/** @file		tcd_decoder.cpp
    @date       2018-08-21
    @version    1.0
    @author     Matthias Seeber
    @brief		TCD Decoder object evaluating incoming TCD messages
*******************************************************************************/

#include "tcd_decoder.h"

/* proper init */

void decoder::init()
{
  uint32_t i;  // a temporary index variable

  /* construct traversable paramID lists                                  -- Recall, Key Event */

  for(i = 0; i < lst_recall_length; i++)
  {
    m_listTraversal[0].add(
        paramIds_recall[i]);  // construct ID list for recall list traversal (former serial/select mode)
  }

  for(i = 0; i < lst_keyEvent_length; i++)
  {
    m_listTraversal[1].add(
        paramIds_keyEvent[i]);  // construct ID list for key event traversal (former serial/select mode)
  }
}

/* evaluate MIDI status and return TCD Command ID                           -- 0 - ignore, >0 - valid tcd commands, see pe_defines_protocol.h */

uint32_t decoder::getCommandId(const uint32_t _status)
{
  return tcd_protocol[_status];  // lookup in predefined command map, return command id (according to MIDI STATUS)
}

/* TCD argument parsing                                                     -- no safety mechanisms, data bytes are expected within [0...127] range! */

uint32_t decoder::unsigned14(const uint32_t _data0, const uint32_t _data1)
{
  return ((_data0 << 7) + _data1);  // parse and return an unsigned 14-bit value according to MIDI DATA bytes
}

int32_t decoder::signed14(const uint32_t _data0, const uint32_t _data1)
{
  return (m_getSign[_data0 >> 6]
          * static_cast<int32_t>(unsigned14(
                (_data0 & 63), _data1)));  // parse and return a signed 14-bit value according to MIDI DATA bytes
}

void decoder::unsigned28upper(const uint32_t _data0, const uint32_t _data1)
{
  m_sign = 1;  // overwrite sign (always positive)
  m_value = unsigned14(_data0, _data1)
      << 14;  // parse and hold an unsigned 14-bit value for the upper 14 bits of 28 bit value according to MIDI DATA bytes
}

void decoder::signed28upper(const uint32_t _data0, const uint32_t _data1)
{
  m_sign = m_getSign[_data0 >> 6];  // determine sign according to MSB of first MIDI DATA byte
  m_value = unsigned14((_data0 & 63), _data1)
      << 14;  // parse and hold an unsigned 13-bit value for the upper 14 bits of 28 bit value according to MIDI DATA bytes
}

int32_t decoder::apply28lower(const uint32_t _data0, const uint32_t _data1)
{
  m_value += unsigned14(
      _data0, _data1);  // parse an unsigned 14-bit value according to MIDI DATA bytes and add it to current value
  return (m_sign * static_cast<int32_t>(m_value));  // parse resulting value (sign * magnitude) and return it
}

/* handle TCD selection by event-based evaluation                           -- check ID against FROM, TO (bit twiddling used in order to avoid multiple conditional statements) */

uint32_t decoder::selectionEvent(const uint32_t _from, const uint32_t _to, const uint32_t _id)
{
  m_event[1] = static_cast<int32_t>(_id) - static_cast<int32_t>(_from);  // first step: ID - FROM
  m_event[2] = static_cast<int32_t>(_to) - static_cast<int32_t>(_id);    // second step: TO - ID
  m_event[3] = m_event[1] | m_event[2];                                  // third step: first or second
  m_event[4] = m_event[1] & m_event[2];                                  // fourth step: first and second
  return (0 > m_event[3 + m_event[0]]
              ? 0
              : 1);  // fifth step: take element according to FROM < TO ? (positive values mean ID is selected)
}

/* decoder list traversal                                                   -- recall, key event */

Parameters decoder::traverseRecall()
{
  /* monophonic implementation                                            -- compatible with current Reaktor5 Rendering Engine */
  const auto id = m_listTraversal[0].m_data[m_listIndex];         // get current
  m_listIndex = (m_listIndex + 1) % m_listTraversal[0].m_length;  // increase (and wrap) index
  return id;                                                      // return current
}

Parameters decoder::traverseKeyEvent()
{
  /* polyphonic implementation                                            -- Voice selection needs to be done by sender for first Voice, then it automatically raises for Unison clusters */
  const auto id = m_listTraversal[1].m_data[m_listIndex];         // get current
  m_listIndex = (m_listIndex + 1) % m_listTraversal[1].m_length;  // increase (and wrap) index
  return id;                                                      // return current
}
