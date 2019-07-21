#pragma once

#include "Types.h"
#include <array>

template <typename T, uint32_t numElements> class RingBuffer
{
 public:
  T& push(const T& e)
  {
    T& ret = m_buffer[m_writePos & mask];
    ret = e;
    m_writePos++;
    return ret;
  }

  const T* peek() const
  {
    if(m_writePos > m_readPos)
      return &(m_buffer[m_readPos & mask]);

    return nullptr;
  }

  const T& pop()
  {
    return m_buffer[m_readPos++ & mask];
  }

  bool empty() const
  {
    return m_writePos <= m_readPos;
  }

 private:
  constexpr static uint32_t mask = (numElements - 1);
  std::array<T, numElements> m_buffer;
  uint32_t m_readPos = 0;
  uint32_t m_writePos = 0;
};
