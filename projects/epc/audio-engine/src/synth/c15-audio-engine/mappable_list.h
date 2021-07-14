#pragma once

/******************************************************************************/
/** @file       mappable_list.h
    @date
    @version    1.7-3
    @author     M. Seeber
    @brief      based on a doubly-linked list, but capable of handling multiple
                entry and exit points and therefore multiple "sublists"
    @todo
*******************************************************************************/

#include <stdint.h>

// singular sorted List
template <uint32_t Length> class SortedList
{
 public:
  uint32_t m_assigned = 0;
  inline SortedList()
  {
    reset();
  }
  inline void reset()
  {
    m_assigned = 0;
    m_head = m_tail = -1;
    for(uint32_t i = 0; i < Length; i++)
    {
      m_prev[i] = m_next[i] = -1;
      m_set[i] = false;
    }
  }
  inline void appendElement(const uint32_t _element)
  {
    // two append scenarios:
    if(m_assigned == 0)
    {
      // set first element
      m_head = m_tail = _element;
      m_prev[_element] = m_next[_element] = -1;
    }
    else
    {
      // append subsequent element
      const int32_t last = m_tail;
      m_next[last] = m_tail = _element;
      m_prev[_element] = last;
      m_next[_element] = -1;
    }
    m_set[_element] = true;
    m_assigned++;
  }
  inline void insertElement(const uint32_t _element)
  {
    // four insert scenarios:
    if(m_assigned == 0)
    {
      // set first element
      m_head = m_tail = _element;
      m_prev[_element] = m_next[_element] = -1;
    }
    else if(m_head > _element)
    {
      // prepend element (before head)
      const int32_t first = m_head;
      m_prev[first] = m_head = _element;
      m_prev[_element] = -1;
      m_next[_element] = first;
    }
    else if(m_tail < _element)
    {
      // append subsequent element
      const int32_t last = m_tail;
      m_next[last] = m_tail = _element;
      m_prev[_element] = last;
      m_next[_element] = -1;
    }
    else
    {
      // find element position between head and tail
      for(int32_t entry = m_head; m_next[entry] != -1; entry = m_next[entry])
      {
        if((entry < _element) && (m_next[entry] > _element))
        {
          // found two neighboring entries for insertion
          m_next[_element] = m_next[entry];
          m_next[entry] = _element;
          m_prev[_element] = entry;
          m_prev[m_next[_element]] = _element;
          break;
        }
      }
    }
    m_set[_element] = true;
    m_assigned++;
  }
  inline void removeElement(const uint32_t _element)
  {
    // one invalid removal scenario:
    if(m_assigned == 0)
    {
      // ignore removal from empty list
      return;
    }
    // four valid removal scenarios:
    const int32_t prev = m_prev[_element], next = m_next[_element];
    m_prev[_element] = m_next[_element] = -1;
    const uint32_t condition = (prev == -1 ? 1 : 0) + (next == -1 ? 2 : 0);
    switch(condition)
    {
      case 0:
        // two neighbors, element is in list of at least length 3
        m_next[prev] = next;
        m_prev[next] = prev;
        break;
      case 1:
        // one neighbor, element is first in list of at least length 2
        m_head = next;
        m_prev[next] = -1;
        break;
      case 2:
        // one neighbor, element is last in list of at least length 2
        m_tail = prev;
        m_next[prev] = -1;
        break;
      case 3:
        // no neighbors, element is last in list
        m_head = m_tail = -1;
        break;
    }
    m_set[_element] = false;
    m_assigned--;
  }
  inline const bool isFirstElement(const uint32_t _element)
  {
    return m_head == _element;
  }
  inline const int32_t getFirstElement()
  {
    return m_head;
  }
  inline const bool isLastElement(const uint32_t _element)
  {
    return m_tail == _element;
  }
  inline const int32_t getLastElement()
  {
    return m_tail;
  }
  inline bool containsElement(const uint32_t _element)
  {
    return m_set[_element];
  }

 private:
  int32_t m_prev[Length] = {}, m_next[Length] = {}, m_head = -1, m_tail = -1;
  bool m_set[Length] = {};
};

// mappable list (multiple assignments)
// specify number of sublists and overall length (number of assignable items)
template <uint32_t Sublists, uint32_t Length> class MappableList
{
 public:
  // constructor
  inline MappableList()
  {
    reset();
  }
  // reset methods: complete or sublist only
  inline void reset()
  {
    // manipulate entry and exit points
    for(int32_t i = 0; i < Sublists; i++)
    {
      const bool s = i == 0;
      m_head[i] = s ? 0 : -1;
      m_tail[i] = s ? m_last : -1;
    }
    // place every element to sublist 0 (serving as "unassigned" sublist)
    for(int32_t i = 0; i < Length; i++)
    {
      m_prev[i] = i - 1;
      m_next[i] = i + 1;
      m_list[i] = 0;
      m_index = m_next[m_last] = -1;
    }
  }
  inline void reset(const uint32_t _listId)
  {
    // reassigning every sublist element to sublist zero
    while(m_head[_listId] != -1)
    {
      reassign(m_head[_listId], 0);
    }
  }
  // reassign method: move an element from one list to another
  inline void reassign(const uint32_t _elementId, const uint32_t _listId)
  {
    // only if element actually changes sublist affiliation
    if(m_list[_elementId] != _listId)
    {
      removeElement(_elementId);
      addElement(_elementId, _listId);
    }
  }
  // traversal methods: facilitate sublist iteration from parent scope
  inline int32_t first(const uint32_t _listId)
  {
    // provide the sublist entry point at start of traversal
    m_index = m_head[_listId];
    return m_index;
  }
  inline bool running()
  {
    // check validity of sublist traversal at current element
    return m_index != -1;
  }
  inline int32_t next()
  {
    // provide the next sublist element
    m_index = m_next[m_index];
    return m_index;
  }

 private:
  // private data arrays: prev and next are pseudo-pointers, list stores element affiliation, head and tail are entry/exit points
  int32_t m_prev[Length], m_next[Length], m_list[Length], m_head[Sublists], m_tail[Sublists], m_index = {};
  const int32_t m_last = Length - 1;
  // basic methods in order to facilitate reassign method
  inline void addElement(const uint32_t _elementId, const uint32_t _listId)
  {
    // two possible scenarios
    if(m_head[_listId] == -1)
    {
      // either first element of currently empty sublist
      m_head[_listId] = m_tail[_listId] = _elementId;
      m_prev[_elementId] = m_next[_elementId] = -1;
    }
    else
    {
      // or next element of non-empty sublist (so, there is a last element of sublist)
      const int32_t last = m_tail[_listId];
      m_next[last] = m_tail[_listId] = _elementId;
      m_prev[_elementId] = last;
      m_next[_elementId] = -1;
    }
    // either way, element now belongs to sublist
    m_list[_elementId] = _listId;
  }
  inline void removeElement(const uint32_t _elementId)
  {
    // first, gather some information about element neighbors and sublist affiliation
    const int32_t prev = m_prev[_elementId], next = m_next[_elementId], listId = m_list[_elementId];
    // four possible scenarios
    const uint32_t condition = (prev == -1 ? 1 : 0) + (next == -1 ? 2 : 0);
    switch(condition)
    {
      case 0:
        // two neighbors, element is in sublist of at least length 3
        m_next[prev] = next;
        m_prev[next] = prev;
        break;
      case 1:
        // one neighbor, element is first in sublist of at least length 2
        m_head[listId] = next;
        m_prev[next] = -1;
        break;
      case 2:
        // one neighbor, element is last in sublist of at least length 2
        m_tail[listId] = prev;
        m_next[prev] = -1;
        break;
      case 3:
        // no neighbors, element is last in sublist
        m_head[listId] = m_tail[listId] = -1;
        break;
    }
    // either way, element now disappears
    m_prev[_elementId] = m_next[_elementId] = m_list[_elementId] = -1;
  }
};
